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

#include <sstream>
#include <cstdint>
#include <cmath>
#include <cassert>

#include "CError.h"
#include "CPalette.h"
#include "CChunkyImage.h"


CChunkyImage CChunkyImageFactory::GetImage(Geometry area, const string& size) const
{
  Image img(_imageRGB, area);
  CChunkyImage subImg;
  subImg._imageRGB = img;

  if (size != "!") {
    // resize : width must be a multiple of 16!
    Geometry sz(size);
    subImg._imageRGB.sample(sz);
    const auto newWidth = subImg._imageRGB.size().width();
    const auto mod = newWidth % 16;
    // if size's not forced
    if (size.find("!") == std::string::npos) {
      if (mod != 0) { //Fit the image in a width multiple of 16
        sz.width(newWidth - mod);
        subImg._imageRGB = img;
        subImg._imageRGB.sample(sz);
      }
    }
    //if size's forced
    else {
      if (mod != 0) {
        string msg("When size if forced, width must be multiple of 16.");
        throw CError(msg);
      }
    }
  }
  subImg._palette = _palette;

  const auto nbPixels = subImg._imageRGB.size().width() * subImg._imageRGB.size().height();
  MagickCore::PixelPacket* pixel = subImg._imageRGB.getPixels(0, 0, subImg._imageRGB.size().width(), subImg._imageRGB.size().height());
  for (unsigned int i = 0; i < nbPixels; i++)
  {
    rgba8Bits_t color{ pixel->red , pixel->green, pixel->blue };
    ++pixel;
    
    const auto hColor = std::find(subImg._palette.begin(), subImg._palette.end(), color);
    if (hColor == subImg._palette.end()) {
        throw CError("Palette is too small.");
    }
    else {
      const auto idx = static_cast<uint8_t>(hColor - subImg._palette.begin());
      subImg._imageIdx.push_back(idx);
    }
  }

  subImg._isInitialized = true;
  return subImg;
}

void CChunkyImageFactory::Init(const Image& img, const unsigned int nbColors, const bool dither, const CPalette& paletteSpace)
{
  if (nbColors > OCS_MAX_COLORS || nbColors < 2) {
    std::ostringstream maxColors;
    maxColors << OCS_MAX_COLORS;
    string msg("Number of colors must be between 2 and " + maxColors.str() + ".");
    throw CError(msg);
  }

  _imageRGB = img;
 
  // constrain the colors to the provided Palette
  const auto nbPixels = _imageRGB.size().width() * _imageRGB.size().height();
  _imageRGB.modifyImage();
  MagickCore::PixelPacket* pixel = _imageRGB.getPixels(0, 0, _imageRGB.size().width(), _imageRGB.size().height());
  for (std::size_t i = 0u; i < nbPixels; ++i)
  {
    const rgba8Bits_t curColor{ pixel->red , pixel->green, pixel->blue };
    const auto nearestAmigaColor = paletteSpace.GetNearestColor(curColor);
    pixel->red = nearestAmigaColor.r << (8 * (sizeof(pixel->red) - 1));
    pixel->green = nearestAmigaColor.g << (8 * (sizeof(pixel->green) - 1));
    pixel->blue = nearestAmigaColor.b << (8 * (sizeof(pixel->blue) - 1));
    ++pixel;
  }
  _imageRGB.syncPixels();

  // quantize image to the nb colors provided
  _imageRGB.quantizeColors(nbColors);
  _imageRGB.quantizeDither(dither);
  _imageRGB.quantizeDitherMethod(Magick::FloydSteinbergDitherMethod);
  //_imageRGB.orderedDither("o3x3,2");
  _imageRGB.quantize();

  _palette = CPaletteFactory::GetInstance().GetUniqueColors(_imageRGB);
}

void CChunkyImage::Save(const string& filename)
{
  _imageRGB.magick("PNG");
  _imageRGB.defineSet("png:color-type", "2");
  _imageRGB.defineSet("png:bit-depth", "8");
  _imageRGB.defineSet("png:format", "png24");
  _imageRGB.write(filename);
}
