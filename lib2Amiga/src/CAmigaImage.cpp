/*
*  Copyright (C) 2014-2021 Christophe Meneboeuf <christophe@xtof.info>
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "libamivideo/amivideotypes.h"
#include "libamivideo/viewportmode.h"
#include "libamivideo/screen.h"
#include "libilbm/ilbmimage.h"
#include "libilbm/bitmapheader.h"
#include "libilbm/interleave.h"
#include "libilbm/ilbm.h"

#include "CError.h"

#include "CAmigaImage.h"


void CAmigaImage::Init( CChunkyImage& image )
{
    //Image width has to be a multiple of 16
    if (image.GetWidth() % 16 != 0) {
        throw CError("Image must have a width multiple of 16.");
    }

    //Allocating memory for bitplanes
    std::size_t bitplaneSz = (image.GetWidth()*image.GetHeight()) >> 3;
    if (_pBitplanes != nullptr) {
      delete[] _pBitplanes;
    }
    _pBitplanes = new uint8_t[BITPLANE_DEPTH_MAX * bitplaneSz];

    //Setting up the "viewport"
    _viewport.bitplaneDepth = { static_cast<unsigned int>(std::ceil(std::log2(image.GetPalette().size()))) };
    _viewport.viewportMode = 0;
    _viewport.width = static_cast<uint16_t>(image.GetWidth());
    _viewport.height = static_cast<uint16_t>(image.GetHeight());
    for (auto i = 0; i < BITPLANE_DEPTH_MAX; i++) {
        _viewport.bitplanes[i] = &_pBitplanes[i*bitplaneSz];
    }
    amiVideo_initScreen(_screen, _viewport.width, _viewport.height, _viewport.bitplaneDepth, OCS_COLORS_PER_CHANNEL, _viewport.viewportMode);
    amiVideo_setScreenBitplanePointers(_screen, _viewport.bitplanes);//Set the bitplane pointers to the pointers in the viewport
    
    //Setting up the input palette
    _screen->palette.chunkyFormat.numOfColors = static_cast<unsigned>(image.GetPalette().size());
    amiVideo_setChunkyPaletteColors(&(_screen->palette), (amiVideo_OutputColor*)(image.GetPalette().data()), static_cast<unsigned>(image.GetPalette().size()));

    //Setting up the image data
    unsigned int pitch = 0; // !! Ask the author of libilbm for a hint to the correct value...
    amiVideo_setScreenUncorrectedChunkyPixelsPointer(_screen, (amiVideo_UByte*)(image.GetPixels().data()), pitch);

    //CONVERTION!!
    amiVideo_convertChunkyColorsToBitplaneFormat(&(_screen->palette));
    amiVideo_convertScreenChunkyPixelsToBitplanes(_screen);    
    
    _isInitialized = true;
}


CAmigaImage::CAmigaImage() :
  _screen(new amiVideo_Screen)
{   }

CAmigaImage::~CAmigaImage(void)
{
    amiVideo_cleanupScreen(_screen);
    if (_screen != nullptr) {
      delete _screen;
    }
    if (_pBitplanes != nullptr) {
        delete[] _pBitplanes;
    }
}


void CAmigaImage::Save(const string & filepath)
{
    ILBM_Image *image = ILBM_createImage("ILBM");

    //Header
    ILBM_BitMapHeader *header = ILBM_createBitMapHeader(); //Create bitmap header properties
    header->w = _viewport.width;
    header->h = _viewport.height;
    header->x = 0;
    header->y = 0;
    header->nPlanes = _viewport.bitplaneDepth;
    header->masking = ILBM_MSK_NONE;
    header->compression = ILBM_CMP_NONE;
    header->transparentColor = 0;
    header->xAspect = 11;
    header->yAspect = 10;
    header->pageWidth = _viewport.width;
    header->pageHeight = _viewport.height;
    image->bitMapHeader = header; //Attach bitmap header to the image
    
    //Interleaving the data
    IFF_UByte* imageData = ILBM_interleaveFromBitplaneMemory(image, _viewport.bitplanes); //!!! imageData HAS TO BE FREED !!!
    if (imageData == NULL) {
        throw CError("Cannot allocate memory for bitplan conversion.");
    }

    //Palette
    ILBM_ColorMap* colorMap = ILBM_createColorMap();  //must be freed using IFF_free()
    ILBM_ColorRegister *colorRegister;
    amiVideo_Color *color = _screen->palette.bitplaneFormat.color;
    for (unsigned int i = 0; i < _screen->palette.bitplaneFormat.numOfColors; i++) {
        colorRegister = ILBM_addColorRegisterInColorMap(colorMap);
        colorRegister->red = color->r;
        colorRegister->green = color->g;
        colorRegister->blue = color->b;
        color++;
    }
    image->colorMap = colorMap;

    //Viewport. Optional??
    ILBM_Viewport *viewport = ILBM_createViewport(); //must be freed using IFF_free()
    viewport->viewportMode = _screen->viewportMode;
    image->viewport = viewport;

    //Adding data to ILBM image
    //Attach data to the body chunk
    auto rowSize = ILBM_calculateRowSize(image) * header->nPlanes;
    IFF_RawChunk* body  = IFF_createRawChunk("BODY");
    IFF_setRawChunkData(body, imageData, image->bitMapHeader->h*rowSize);
    image->body = body;

    IFF_Form * output = ILBM_convertImageToForm(image);
    if (ILBM_write(filepath.data(), (IFF_Chunk*)output) != TRUE) {
        throw CError("Cannot write to the output file.");
    }

    //Free memory
    free(imageData);
}

