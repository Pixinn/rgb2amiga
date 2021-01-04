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

#include "palette.h"
#include <stdlib.h>
#include <string.h>
#include "viewportmode.h"

static unsigned int determineNumOfColors(unsigned int bitplaneDepth)
{
    switch(bitplaneDepth)
    {
	case 1:
	    return 2;
	case 2:
	    return 4;
	case 3:
	    return 8;
	case 4:
	    return 16;
	case 5:
	    return 32;
	case 6:
	    return 64;
	case 7:
	    return 128;
	case 8:
	    return 256;
	default:
	    return 0;
    }
}

void amiVideo_initPalette(amiVideo_Palette *palette, unsigned int bitplaneDepth, unsigned int bitsPerColorChannel, amiVideo_Long viewportMode)
{
    /* Assign values */
    palette->bitplaneFormat.viewportMode = viewportMode;
    
    /* Allocate memory for bitplane colors */

    palette->bitplaneFormat.bitsPerColorChannel = bitsPerColorChannel;
    palette->bitplaneFormat.numOfColors = determineNumOfColors(bitplaneDepth);
    palette->bitplaneFormat.color = (amiVideo_Color*)malloc(palette->bitplaneFormat.numOfColors * sizeof(amiVideo_Color));

    /* Allocate memory for chunky colors */
    
    if(amiVideo_checkExtraHalfbrite(viewportMode))
        palette->chunkyFormat.numOfColors = 2 * palette->bitplaneFormat.numOfColors; /* Extra halfbrite screen mode has double the amount of colors */
    else
        palette->chunkyFormat.numOfColors = palette->bitplaneFormat.numOfColors;
    
    palette->chunkyFormat.color = (amiVideo_OutputColor*)malloc(palette->chunkyFormat.numOfColors * sizeof(amiVideo_OutputColor));
}

void amiVideo_cleanupPalette(amiVideo_Palette *palette)
{
    free(palette->bitplaneFormat.color);
    free(palette->chunkyFormat.color);
}

void amiVideo_setBitplanePaletteColors(amiVideo_Palette *palette, amiVideo_Color *color, unsigned int numOfColors)
{
    unsigned int numOfRemainingColors = palette->bitplaneFormat.numOfColors - numOfColors;
    
    /* Copy the given colors */
    memcpy(palette->bitplaneFormat.color, color, numOfColors * sizeof(amiVideo_Color));
    
    if(numOfRemainingColors > 0)
	memset(palette->bitplaneFormat.color + numOfColors, '\0', numOfRemainingColors * sizeof(amiVideo_Color)); /* Set the remaining ones to 0 */
}

void amiVideo_setChunkyPaletteColors(amiVideo_Palette *palette, amiVideo_OutputColor *color, unsigned int numOfColors)
{
    unsigned int numOfRemainingColors = palette->chunkyFormat.numOfColors - numOfColors;
    
    /* Copy the given colors */
    memcpy(palette->chunkyFormat.color, color, numOfColors * sizeof(amiVideo_OutputColor));
    
    if(numOfRemainingColors > 0)
	memset(palette->chunkyFormat.color + numOfColors, '\0', numOfRemainingColors * sizeof(amiVideo_OutputColor)); /* Set the remaining ones to 0 */
}

void amiVideo_convertBitplaneColorsToChunkyFormat(amiVideo_Palette *palette)
{
    /* We must convert color channels that consist do not consist of 8 bits */
    unsigned int i;
    int shift = 8 - palette->bitplaneFormat.bitsPerColorChannel;

    for(i = 0; i < palette->bitplaneFormat.numOfColors; i++)
    {
        amiVideo_Color *sourceColor = &palette->bitplaneFormat.color[i];
        amiVideo_OutputColor *targetColor = &palette->chunkyFormat.color[i];

        targetColor->r = sourceColor->r << shift;
        targetColor->g = sourceColor->g << shift;
        targetColor->b = sourceColor->b << shift;
        targetColor->a = '\0';
    }
    
    /* For extra half brite screen modes we must append half of the color values of the original color register values */
    
    if(amiVideo_checkExtraHalfbrite(palette->bitplaneFormat.viewportMode))
    {
	unsigned int i;
	
	for(i = 0; i < palette->bitplaneFormat.numOfColors; i++)
	{
	    amiVideo_OutputColor *sourceColor = &palette->chunkyFormat.color[i];
	    amiVideo_OutputColor *targetColor = &palette->chunkyFormat.color[i + palette->bitplaneFormat.numOfColors];
	    
	    targetColor->r = sourceColor->r >> 1;
	    targetColor->g = sourceColor->g >> 1;
	    targetColor->b = sourceColor->b >> 1;
	    targetColor->a = '\0';
	}
    }
}

void amiVideo_convertChunkyColorsToBitplaneFormat(amiVideo_Palette *palette)
{
    unsigned int i;

    if (palette->bitplaneFormat.bitsPerColorChannel == 4)
    {
        for (i = 0; i < palette->chunkyFormat.numOfColors; i++)
        {
            amiVideo_OutputColor chunkyColor = palette->chunkyFormat.color[i];
            amiVideo_Color *color = &palette->bitplaneFormat.color[i];
            color->r = (chunkyColor.r / 17) << 4;
            color->g = (chunkyColor.g / 17) << 4;
            color->b = (chunkyColor.b / 17) << 4;
        }
    }
}

amiVideo_UWord *amiVideo_generateRGB4ColorSpecs(const amiVideo_Palette *palette)
{
    unsigned int i;
    int shift = palette->bitplaneFormat.bitsPerColorChannel - 4;
    amiVideo_UWord *colorSpecs = (amiVideo_UWord*)malloc(palette->bitplaneFormat.numOfColors * sizeof(amiVideo_UWord));
    
    for(i = 0; i < palette->bitplaneFormat.numOfColors; i++)
    {
	amiVideo_Color *color = &palette->bitplaneFormat.color[i];
	colorSpecs[i] = ((color->r >> shift) << 8) | ((color->g >> shift) << 4) | (color->b >> shift);
    }
    
    return colorSpecs;
}

amiVideo_ULong *amiVideo_generateRGB32ColorSpecs(const amiVideo_Palette *palette)
{
    unsigned int i;
    unsigned int index = 1;
    int shift = 32 - palette->bitplaneFormat.bitsPerColorChannel;
    
    amiVideo_ULong *colorSpecs = (amiVideo_ULong*)malloc((palette->bitplaneFormat.numOfColors * 3 + 2) * sizeof(amiVideo_ULong));
    
    /* First element's first word is number of colors, second word is the first color to be loaded (which is 0) */
    colorSpecs[0] = palette->bitplaneFormat.numOfColors << 16;
    
    /* Remaining elements are red, green, blue component values for each color register */
    for(i = 0; i < palette->bitplaneFormat.numOfColors; i++)
    {
	amiVideo_Color color = palette->bitplaneFormat.color[i];
	
	colorSpecs[index] = color.r << shift;
	index++;
	colorSpecs[index] = color.g << shift;
	index++;
	colorSpecs[index] = color.b << shift;
	index++;
    }
    
    /* Add 0 termination at the end */
    colorSpecs[index] = 0;
    
    /* Return the generated color specs */
    return colorSpecs;
}
