/*
 * Copyright (c) 2012 Sander van der Burg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "ilbmimage.h"
#include "ilbm.h"
#include <stdlib.h>
#include <libiff/list.h>
#include <libiff/cat.h>
#include <libiff/id.h>
#include <libiff/error.h>

ILBM_Image *ILBM_createImage(char *formType)
{
    ILBM_Image *image = (ILBM_Image*)calloc(1, sizeof(ILBM_Image));
    IFF_createId(image->formType, formType);
    return image;
}

static ILBM_Image *createImageFromForm(IFF_Form *form, char *formType)
{
    ILBM_Image *image = (ILBM_Image*)malloc(sizeof(ILBM_Image));
    
    IFF_createId(image->formType, formType);
    image->bitMapHeader = (ILBM_BitMapHeader*)IFF_getChunkFromForm(form, "BMHD");
    image->colorMap = (ILBM_ColorMap*)IFF_getChunkFromForm(form, "CMAP");
    image->point2d = (ILBM_Point2D*)IFF_getChunkFromForm(form, "GRAB");
    image->destMerge = (ILBM_DestMerge*)IFF_getChunkFromForm(form, "DEST");
    image->sprite = (ILBM_Sprite*)IFF_getChunkFromForm(form, "SPRT");
    image->viewport = (ILBM_Viewport*)IFF_getChunkFromForm(form, "CAMG");
    image->colorRange = (ILBM_ColorRange**)IFF_getChunksFromForm(form, "CRNG", &image->colorRangeLength);
    image->drange = (ILBM_DRange**)IFF_getChunksFromForm(form, "DRNG", &image->drangeLength);
    image->cycleInfo = (ILBM_CycleInfo**)IFF_getChunksFromForm(form, "CCRT", &image->cycleInfoLength);
    image->body = (IFF_RawChunk*)IFF_getChunkFromForm(form, "BODY");
    image->bitplanes = (IFF_RawChunk*)IFF_getChunkFromForm(form, "ABIT");
    
    return image;
}

ILBM_Image **ILBM_extractImages(IFF_Chunk *chunk, unsigned int *imagesLength)
{
    unsigned int ilbmFormsLength;
    IFF_Form **ilbmForms = IFF_searchForms(chunk, "ILBM", &ilbmFormsLength);
    unsigned int pbmFormsLength;
    IFF_Form **pbmForms = IFF_searchForms(chunk, "PBM ", &pbmFormsLength);
    unsigned int acbmFormsLength;
    IFF_Form **acbmForms = IFF_searchForms(chunk, "ACBM", &acbmFormsLength);
    
    *imagesLength = ilbmFormsLength + pbmFormsLength + acbmFormsLength;
    
    if(*imagesLength == 0)
    {
        IFF_error("No form with formType: 'ILBM', 'PBM ' or 'ACBM' found!\n");
        return NULL;
    }
    else
    {
        ILBM_Image **images = (ILBM_Image**)malloc(*imagesLength * sizeof(ILBM_Image*));
        unsigned int offset, i;
        
        /* Extract all ILBM images */
        for(i = 0; i < ilbmFormsLength; i++)
        {
            IFF_Form *ilbmForm = ilbmForms[i];
            images[i] = createImageFromForm(ilbmForm, "ILBM");
        }
        
        offset = ilbmFormsLength;
        
        /* Extract all PBM images */
        for(i = 0; i < pbmFormsLength; i++)
        {
            IFF_Form *pbmForm = pbmForms[i];
            images[offset + i] = createImageFromForm(pbmForm, "PBM ");
        }
        
        offset += pbmFormsLength;
        
        /* Extract all ACBM images */
        for(i = 0; i < acbmFormsLength; i++)
        {
            IFF_Form *acbmForm = acbmForms[i];
            images[offset + i] = createImageFromForm(acbmForm, "ACBM");
        }
        
        /* Clean up stuff */
        free(ilbmForms);
        free(pbmForms);
        free(acbmForms);
        
        /* Return generated images array */
        return images;
    }
}

IFF_Form *ILBM_convertImageToForm(ILBM_Image *image)
{
    IFF_Form *form = IFF_createForm(image->formType);
    unsigned int i;
    
    if(image->bitMapHeader != NULL)
        IFF_addToForm(form, (IFF_Chunk*)image->bitMapHeader);
    
    if(image->colorMap != NULL)
        IFF_addToForm(form, (IFF_Chunk*)image->colorMap);
    
    if(image->point2d != NULL)
        IFF_addToForm(form, (IFF_Chunk*)image->point2d);

    if(image->destMerge != NULL)
        IFF_addToForm(form, (IFF_Chunk*)image->destMerge);
    
    if(image->sprite != NULL)
        IFF_addToForm(form, (IFF_Chunk*)image->sprite);
    
    if(image->viewport != NULL)
        IFF_addToForm(form, (IFF_Chunk*)image->viewport);
    
    for(i = 0; i < image->colorRangeLength; i++)
        IFF_addToForm(form, (IFF_Chunk*)image->colorRange[i]);
    
    for(i = 0; i < image->drangeLength; i++)
        IFF_addToForm(form, (IFF_Chunk*)image->drange[i]);
    
    for(i = 0; i < image->cycleInfoLength; i++)
        IFF_addToForm(form, (IFF_Chunk*)image->cycleInfo[i]);
    
    if(image->body != NULL)
        IFF_addToForm(form, (IFF_Chunk*)image->body);
    
    if(image->bitplanes != NULL)
        IFF_addToForm(form, (IFF_Chunk*)image->bitplanes);
        
    return form;
}

void ILBM_freeImage(ILBM_Image *image)
{
    free(image->colorRange);
    free(image->drange);
    free(image->cycleInfo);
    free(image);
}

void ILBM_freeImages(ILBM_Image **images, const unsigned int imagesLength)
{
    unsigned int i;
    
    for(i = 0; i < imagesLength; i++)
        ILBM_freeImage(images[i]);

    free(images);
}

int ILBM_checkImage(const ILBM_Image *image)
{
    if(image->bitMapHeader == NULL)
    {
        IFF_error("Error: no bitmap header defined!\n");
        return FALSE;
    }
    
    return TRUE;
}

int ILBM_checkImages(const IFF_Chunk *chunk, ILBM_Image **images, const unsigned int imagesLength)
{
    unsigned int i;
    
    /* First, check the ILBM file for corectness */
    if(!ILBM_check(chunk))
        return FALSE;
    
    /* Check the individual images inside the IFF file */
    for(i = 0; i < imagesLength; i++)
    {
        if(!ILBM_checkImage(images[i]))
            return FALSE;
    }
    
    /* Everything seems to be correct */
    return TRUE;
}

void ILBM_addColorRangeToImage(ILBM_Image *image, ILBM_ColorRange *colorRange)
{
    image->colorRange = (ILBM_ColorRange**)realloc(image->colorRange, (image->colorRangeLength + 1) * sizeof(ILBM_ColorRange*));
    image->colorRange[image->colorRangeLength] = colorRange;
    image->colorRangeLength++;
}

void ILBM_addDRangeToImage(ILBM_Image *image, ILBM_DRange *drange)
{
    image->drange = (ILBM_DRange**)realloc(image->drange, (image->drangeLength + 1) * sizeof(ILBM_DRange*));
    image->drange[image->drangeLength] = drange;
    image->drangeLength++;
}

void ILBM_addCycleInfoToImage(ILBM_Image *image, ILBM_CycleInfo *cycleInfo)
{
    image->cycleInfo = (ILBM_CycleInfo**)realloc(image->cycleInfo, (image->cycleInfoLength + 1) * sizeof(ILBM_CycleInfo*));
    image->cycleInfo[image->cycleInfoLength] = cycleInfo;
    image->cycleInfoLength++;
}

int ILBM_imageIsILBM(const ILBM_Image *image)
{
    return (IFF_compareId(image->formType, "ILBM") == 0);
}

int ILBM_imageIsACBM(const ILBM_Image *image)
{
    return (IFF_compareId(image->formType, "ACBM") == 0);
}

int ILBM_imageIsPBM(const ILBM_Image *image)
{
    return (IFF_compareId(image->formType, "PBM ") == 0);
}

unsigned int ILBM_calculateRowSize(const ILBM_Image *image)
{
    unsigned int rowSizeInWords = image->bitMapHeader->w / 16;
    
    if(image->bitMapHeader->w % 16 != 0)
        rowSizeInWords++;
    
    return (rowSizeInWords * 2);
}

ILBM_ColorMap *ILBM_generateGrayscaleColorMap(const ILBM_Image *image)
{
    ILBM_ColorMap *colorMap = ILBM_createColorMap();
    
    if(colorMap != NULL)
    {
        unsigned int numOfColors = ILBM_calculateNumOfColors(image->bitMapHeader);
        unsigned int i;
    
        for(i = 0; i < numOfColors; i++)
        {
            ILBM_ColorRegister *colorRegister = ILBM_addColorRegisterInColorMap(colorMap);
            unsigned int value = i * 0xff / (numOfColors - 1);
            
            colorRegister->red = value;
            colorRegister->green = value;
            colorRegister->blue = value;
        }
    }
    
    return colorMap;
}
