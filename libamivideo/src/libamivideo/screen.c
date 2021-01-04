#include "screen.h"
#include <stdlib.h>
#include <string.h>
#include "viewportmode.h"

#define TRUE 1
#define FALSE 0

void amiVideo_initScreen(amiVideo_Screen *screen, amiVideo_Word width, amiVideo_Word height, unsigned int bitplaneDepth, unsigned int bitsPerColorChannel, amiVideo_Long viewportMode)
{
    unsigned int scanLineSizeInWords;

    /* Assign values */
    screen->width = width;
    screen->height = height;
    screen->bitplaneDepth = bitplaneDepth;
    screen->viewportMode = viewportMode;
    
    /* Set allocation bits to FALSE */
    screen->bitplaneFormat.memoryAllocated = FALSE;
    screen->uncorrectedChunkyFormat.memoryAllocated = FALSE;
    screen->uncorrectedRGBFormat.memoryAllocated = FALSE;
    
    /* Sets the palette */
    amiVideo_initPalette(&screen->palette, bitplaneDepth, bitsPerColorChannel, viewportMode);
    
    /* Calculate the pitch of the bitplanes. The width in bytes is rounded to the nearest word boundary */
    
    scanLineSizeInWords = screen->width / 16;
    
    if(screen->width % 16 != 0)
        scanLineSizeInWords++;

    screen->bitplaneFormat.pitch = scanLineSizeInWords * 2;
}

amiVideo_Screen *amiVideo_createScreen(amiVideo_Word width, amiVideo_Word height, unsigned int bitplaneDepth, unsigned int bitsPerColorChannel, amiVideo_Long viewportMode)
{
    amiVideo_Screen *screen = (amiVideo_Screen*)malloc(sizeof(amiVideo_Screen));
    
    if(screen != NULL)
	amiVideo_initScreen(screen, width, height, bitplaneDepth, bitsPerColorChannel, viewportMode);
    
    /* Return the allocated screen */
    return screen;
}

void amiVideo_cleanupScreen(amiVideo_Screen *screen)
{
    amiVideo_cleanupPalette(&screen->palette);

    if(screen->uncorrectedChunkyFormat.memoryAllocated)
	free(screen->uncorrectedChunkyFormat.pixels);
    
    if(screen->uncorrectedRGBFormat.memoryAllocated)
	free(screen->uncorrectedRGBFormat.pixels);
}

void amiVideo_freeScreen(amiVideo_Screen *screen)
{
    amiVideo_cleanupScreen(screen);
    free(screen);
}

int amiVideo_calculateCorrectedWidth(unsigned int lowresPixelScaleFactor, amiVideo_Long width, amiVideo_Long viewportMode)
{
    if(amiVideo_checkSuperHires(viewportMode))
        return lowresPixelScaleFactor * width / 4;
    else if(amiVideo_checkHires(viewportMode))
        return lowresPixelScaleFactor * width / 2; /* Hires pixels have double the size of super hires pixels */
    else
        return lowresPixelScaleFactor * width; /* Lowres pixels have double the size of hi-res pixels */
}

int amiVideo_calculateCorrectedHeight(unsigned int lowresPixelScaleFactor, amiVideo_Long height, amiVideo_Long viewportMode)
{
    if(amiVideo_checkLaced(viewportMode))
        return lowresPixelScaleFactor * height / 2;
    else
        return lowresPixelScaleFactor * height; /* Non-interlaced screens have double the amount of scanlines */
}

void amiVideo_setLowresPixelScaleFactor(amiVideo_Screen *screen, unsigned int lowresPixelScaleFactor)
{
    screen->correctedFormat.lowresPixelScaleFactor = lowresPixelScaleFactor;
    
    screen->correctedFormat.width = amiVideo_calculateCorrectedWidth(lowresPixelScaleFactor, screen->width, screen->viewportMode);
    screen->correctedFormat.height = amiVideo_calculateCorrectedHeight(lowresPixelScaleFactor, screen->height, screen->viewportMode);
}

void amiVideo_setScreenBitplanePointers(amiVideo_Screen *screen, amiVideo_UByte **bitplanes)
{
    memcpy(screen->bitplaneFormat.bitplanes, bitplanes, screen->bitplaneDepth * sizeof(amiVideo_UByte*));
}

void amiVideo_setScreenBitplanes(amiVideo_Screen *screen, amiVideo_UByte *bitplanes)
{
    amiVideo_UByte *bitplanePointers[AMIVIDEO_MAX_NUM_OF_BITPLANES];
    unsigned int offset = 0;
    unsigned int i;
    
    /* Set bitplane pointers */
    
    for(i = 0; i < screen->bitplaneDepth; i++)
    {
	bitplanePointers[i] = bitplanes + offset;
	offset += screen->bitplaneFormat.pitch * screen->height;
    }
    
    /* Set bitplane pointers */
    amiVideo_setScreenBitplanePointers(screen, bitplanePointers);
}

void amiVideo_setScreenUncorrectedChunkyPixelsPointer(amiVideo_Screen *screen, amiVideo_UByte *pixels, unsigned int pitch)
{
    screen->uncorrectedChunkyFormat.pixels = pixels;
    screen->uncorrectedChunkyFormat.pitch = pitch;
    screen->uncorrectedChunkyFormat.memoryAllocated = FALSE;
}

void amiVideo_setScreenUncorrectedRGBPixelsPointer(amiVideo_Screen *screen, amiVideo_ULong *pixels, unsigned int pitch, int allocateUncorrectedMemory, amiVideo_UByte rshift, amiVideo_UByte gshift, amiVideo_UByte bshift, amiVideo_UByte ashift)
{
    screen->uncorrectedRGBFormat.pixels = pixels;
    screen->uncorrectedRGBFormat.pitch = pitch;
    screen->uncorrectedRGBFormat.memoryAllocated = FALSE;
    screen->uncorrectedRGBFormat.rshift = rshift;
    screen->uncorrectedRGBFormat.gshift = gshift;
    screen->uncorrectedRGBFormat.bshift = bshift;
    screen->uncorrectedRGBFormat.ashift = ashift;
    
    if(allocateUncorrectedMemory)
    {
	screen->uncorrectedChunkyFormat.pitch = screen->width;
	screen->uncorrectedChunkyFormat.pixels = (amiVideo_UByte*)calloc(screen->uncorrectedChunkyFormat.pitch * screen->height, sizeof(amiVideo_UByte));
	screen->uncorrectedChunkyFormat.memoryAllocated = TRUE;
    }
}

void amiVideo_setScreenCorrectedPixelsPointer(amiVideo_Screen *screen, void *pixels, unsigned int pitch, unsigned int bytesPerPixel, int allocateUncorrectedMemory, amiVideo_UByte rshift, amiVideo_UByte gshift, amiVideo_UByte bshift, amiVideo_UByte ashift)
{
    screen->correctedFormat.pixels = pixels;
    screen->correctedFormat.pitch = pitch;
    screen->correctedFormat.bytesPerPixel = bytesPerPixel;
    
    if(allocateUncorrectedMemory)
    {
	screen->uncorrectedChunkyFormat.pitch = screen->width;
	screen->uncorrectedChunkyFormat.pixels = (amiVideo_UByte*)calloc(screen->uncorrectedChunkyFormat.pitch * screen->height, sizeof(amiVideo_UByte));
	screen->uncorrectedChunkyFormat.memoryAllocated = TRUE;
	
	if(bytesPerPixel == 4)
	{
	    screen->uncorrectedRGBFormat.pitch = screen->width * 4;
	    screen->uncorrectedRGBFormat.pixels = (amiVideo_ULong*)calloc(screen->uncorrectedRGBFormat.pitch * screen->height, sizeof(amiVideo_UByte));
	    screen->uncorrectedRGBFormat.memoryAllocated = TRUE;
	    screen->uncorrectedRGBFormat.rshift = rshift;
	    screen->uncorrectedRGBFormat.gshift = gshift;
	    screen->uncorrectedRGBFormat.bshift = bshift;
	    screen->uncorrectedRGBFormat.ashift = ashift;
	}
	else
	    screen->uncorrectedRGBFormat.memoryAllocated = FALSE;
    }
    else
    {
	screen->uncorrectedChunkyFormat.memoryAllocated = FALSE;
	screen->uncorrectedRGBFormat.memoryAllocated = FALSE;
    }
}

static void convertScreenBitplanesToTarget(amiVideo_Screen *screen, int chunky)
{
    unsigned int i;
    
    for(i = 0; i < screen->bitplaneDepth; i++) /* Iterate over each bitplane */
    {
	unsigned int count = 0;
	amiVideo_ULong indexBit = 1 << i;
	amiVideo_UByte *bitplanes = screen->bitplaneFormat.bitplanes[i];
	unsigned int vOffset = 0;
	unsigned int j;
	
	for(j = 0; j < screen->height; j++) /* Iterate over each scan line */
	{
	    unsigned int hOffset = vOffset;
	    unsigned int k;
	    unsigned int pixelCount = 0;
	    
	    for(k = 0; k < screen->bitplaneFormat.pitch; k++) /* Iterate over each byte containing 8 pixels */
	    {
		amiVideo_UByte bitplane = bitplanes[hOffset];
		unsigned char bitmask = 0x80;
		unsigned int l;
		
		for(l = 0; l < 8; l++) /* Iterate over each bit representing a pixel */
		{
		    if(pixelCount < screen->width) /* We must skip the padding bits. If we have already converted sufficient pixels on this scanline, ignore the rest */
		    {
			if(bitplane & bitmask)
			{
			    if(chunky)
			        screen->uncorrectedChunkyFormat.pixels[count] |= indexBit;
			    else
			        screen->uncorrectedRGBFormat.pixels[count] |= indexBit;
			}
			count++;
		    }
		    
		    pixelCount++;
		    bitmask >>= 1;
		}
		
		hOffset++;
	    }
	    
	    /* Skip the padding bytes in the output */
	    if(chunky)
	        count += screen->uncorrectedChunkyFormat.pitch - screen->width; 
	    else
	        count += screen->uncorrectedRGBFormat.pitch / 4 - screen->width; 
	    
	    vOffset += screen->bitplaneFormat.pitch;
	}
    }
}

void amiVideo_convertScreenBitplanesToChunkyPixels(amiVideo_Screen *screen)
{
    convertScreenBitplanesToTarget(screen, TRUE);
}

static amiVideo_ULong convertColorToRGBPixel(const amiVideo_OutputColor *color, amiVideo_UByte rshift, amiVideo_UByte gshift, amiVideo_UByte bshift, amiVideo_UByte ashift)
{
    return (color->r << rshift) | (color->g << gshift) | (color->b << bshift) | (color->a << ashift);
}

void amiVideo_convertScreenChunkyPixelsToRGBPixels(amiVideo_Screen *screen)
{
    unsigned int screenWidthInPixels = screen->uncorrectedRGBFormat.pitch / 4;
    
    if(amiVideo_checkHoldAndModify(screen->viewportMode))
    {
	/* HAM mode has its own decompression technique */
	
	unsigned int i;
	unsigned int offset = 0;
	
	for(i = 0; i < screen->height; i++)
	{
	    unsigned int j;
	    amiVideo_OutputColor previousResult = screen->palette.chunkyFormat.color[0];
	    
	    for(j = 0; j < screenWidthInPixels; j++)
	    {
		amiVideo_UByte byte = screen->uncorrectedChunkyFormat.pixels[offset + j];
		amiVideo_UByte mode = (byte & (0x3 << (screen->bitplaneDepth - 2))) >> (screen->bitplaneDepth - 2);
		amiVideo_UByte index = byte & ~(0x3 << (screen->bitplaneDepth - 2));
		amiVideo_OutputColor result;
		
		if(mode == 0x0) /* Data bits are an index in the color palette */
		    result = screen->palette.chunkyFormat.color[index];
		else if(mode == 0x1) /* Data bits are blue level */
		{
		    result = previousResult;
		    result.b = index << (8 - screen->bitplaneDepth + 2);
		}
		else if(mode == 0x2) /* Data bits are red level */
		{
		    result = previousResult;
		    result.r = index << (8 - screen->bitplaneDepth + 2);
		}
		else if(mode == 0x3) /* Data bits are green level */
		{
		    result = previousResult;
		    result.g = index << (8 - screen->bitplaneDepth + 2);
		}
		
		/* set new pixel on offset + j */
		screen->uncorrectedRGBFormat.pixels[offset + j] = convertColorToRGBPixel(&result, screen->uncorrectedRGBFormat.rshift, screen->uncorrectedRGBFormat.gshift, screen->uncorrectedRGBFormat.bshift, screen->uncorrectedRGBFormat.ashift);
		
		previousResult = result;
	    }
	    
	    offset += screenWidthInPixels;
	}
    }
    else
    {
	/* Normal mode */
	
	unsigned int i;
	
	for(i = 0; i < screenWidthInPixels * screen->height; i++)
	    screen->uncorrectedRGBFormat.pixels[i] = convertColorToRGBPixel(&screen->palette.chunkyFormat.color[screen->uncorrectedChunkyFormat.pixels[i]], screen->uncorrectedRGBFormat.rshift, screen->uncorrectedRGBFormat.gshift, screen->uncorrectedRGBFormat.bshift, screen->uncorrectedRGBFormat.ashift);
    }
}

void amiVideo_convertScreenChunkyPixelsToBitplanes(amiVideo_Screen *screen)
{
    unsigned int i;
    unsigned int bitplaneIndex = 0;
    int bit = 7;
    
    for (i = 0; i < screen->width * screen->height; i++)
    {
        unsigned int j;
        amiVideo_UByte bitmask = 1 << bit;

        for (j = 0; j < screen->bitplaneDepth; j++)
        {
            if (screen->uncorrectedChunkyFormat.pixels[i] & (1 << j)) /* Check if the current bit of the index value is set */
                screen->bitplaneFormat.bitplanes[j][bitplaneIndex] |= bitmask; /* Modify the current bit in the bitplane byte to be 1 and leave the others untouched */
            else
                screen->bitplaneFormat.bitplanes[j][bitplaneIndex] &= ~bitmask; /* Modify the current bit in the bitplane byte to be 0 and leave the others untouched */
        }

        bit--;

        if (bit < 0)
        {
            bit = 7; /* Reset the bit counter */
            bitplaneIndex++; /* Go to the next byte in the bitplane memory */
        }
    }
}

void amiVideo_correctScreenPixels(amiVideo_Screen *screen)
{
    unsigned int i;
    unsigned int sourceOffset = 0, destOffset = 0;
    
    unsigned int repeatHorizontal;
    unsigned int repeatVertical;
    
    amiVideo_UByte *pixels;
    
    /* Calculate how many times we have to horizontally repeat a pixel */
    if(amiVideo_checkSuperHires(screen->viewportMode))
	repeatHorizontal = screen->correctedFormat.lowresPixelScaleFactor / 4;
    else if(amiVideo_checkHires(screen->viewportMode))
        repeatHorizontal = screen->correctedFormat.lowresPixelScaleFactor / 2;
    else
        repeatHorizontal = screen->correctedFormat.lowresPixelScaleFactor;
    
    /* Calculate how many times we have to vertically repeat a scanline */
    
    if(amiVideo_checkLaced(screen->viewportMode))
        repeatVertical = screen->correctedFormat.lowresPixelScaleFactor / 2;
    else
        repeatVertical = screen->correctedFormat.lowresPixelScaleFactor;
    
    /* Check which pixels we have to correct */
    
    if(screen->correctedFormat.bytesPerPixel == 1)
	pixels = screen->uncorrectedChunkyFormat.pixels;
    else
	pixels = (amiVideo_UByte*)screen->uncorrectedRGBFormat.pixels;
    
    /* Do the correction */
    for(i = 0; i < screen->height; i++)
    {
	unsigned int j;
	
	for(j = 0; j < screen->width; j++)
	{
	    unsigned int k;
	    
	    /* Scale the pixel horizontally */
	    for(k = 0; k < repeatHorizontal; k++)
	    {
	        memcpy((amiVideo_UByte*)screen->correctedFormat.pixels + destOffset, pixels + sourceOffset, screen->correctedFormat.bytesPerPixel);
	        destOffset += screen->correctedFormat.bytesPerPixel;
	    }
	
	    sourceOffset += screen->correctedFormat.bytesPerPixel;
	}
	
	destOffset += screen->correctedFormat.pitch - screen->correctedFormat.width * screen->correctedFormat.bytesPerPixel; /* Skip the padding bytes */
	
	/* Non-interlace screen scanlines must be doubled */
	
	for(j = 1; j < repeatVertical; j++)
	{
	    memcpy((amiVideo_UByte*)screen->correctedFormat.pixels + destOffset, (amiVideo_UByte*)screen->correctedFormat.pixels + destOffset - screen->correctedFormat.pitch, screen->correctedFormat.pitch);
	    destOffset += screen->correctedFormat.pitch;
	}
    }
}

void amiVideo_convertScreenBitplanesToRGBPixels(amiVideo_Screen *screen)
{
    if(screen->bitplaneDepth == 24 || screen->bitplaneDepth == 32) /* For true color images we directly convert bitplanes to RGB pixels */
    {
        convertScreenBitplanesToTarget(screen, FALSE);
        amiVideo_reorderRGBPixels(screen);
    }
    else
    {
        /* For lower bitplane depths we first have to compose chunky pixels to determine the actual color values */
        amiVideo_convertBitplaneColorsToChunkyFormat(&screen->palette);
        amiVideo_convertScreenBitplanesToChunkyPixels(screen);
        amiVideo_convertScreenChunkyPixelsToRGBPixels(screen);
    }
}

void amiVideo_convertScreenBitplanesToCorrectedChunkyPixels(amiVideo_Screen *screen)
{
    amiVideo_convertScreenBitplanesToChunkyPixels(screen);
    amiVideo_correctScreenPixels(screen);
}

void amiVideo_convertScreenBitplanesToCorrectedRGBPixels(amiVideo_Screen *screen)
{
    amiVideo_convertScreenBitplanesToRGBPixels(screen);
    amiVideo_correctScreenPixels(screen);
}

void amiVideo_convertScreenChunkyPixelsToCorrectedRGBPixels(amiVideo_Screen *screen)
{
    amiVideo_convertScreenChunkyPixelsToRGBPixels(screen);
    amiVideo_correctScreenPixels(screen);
}

amiVideo_ColorFormat amiVideo_autoSelectColorFormat(const amiVideo_Screen *screen)
{
    if(amiVideo_checkHoldAndModify(screen->viewportMode) || screen->bitplaneDepth == 24 || screen->bitplaneDepth == 32)
        return AMIVIDEO_RGB_FORMAT;
    else
        return AMIVIDEO_CHUNKY_FORMAT;
}

static void reorderPixelBytes(amiVideo_Screen *screen, amiVideo_UByte rshift, amiVideo_UByte gshift, amiVideo_UByte bshift, amiVideo_UByte ashift)
{
    unsigned int i;
    
    for(i = 0; i < screen->uncorrectedRGBFormat.pitch / 4 * screen->height; i++)
    {
        amiVideo_ULong pixel = screen->uncorrectedRGBFormat.pixels[i];
        amiVideo_OutputColor color;
        
        color.r = (pixel >> rshift) & 0xff;
        color.g = (pixel >> gshift) & 0xff;
        color.b = (pixel >> bshift) & 0xff;
        color.a = (pixel >> ashift) & 0xff;
        
        convertColorToRGBPixel(&color, screen->uncorrectedRGBFormat.rshift, screen->uncorrectedRGBFormat.gshift, screen->uncorrectedRGBFormat.bshift, screen->uncorrectedRGBFormat.ashift);
    }
}

void amiVideo_reorderRGBPixels(amiVideo_Screen *screen)
{
    /* Reorder the bytes if the real display uses a different order */
    if(screen->bitplaneDepth == 24 && (screen->uncorrectedRGBFormat.ashift != 24 || screen->uncorrectedRGBFormat.rshift != 16 || screen->uncorrectedRGBFormat.gshift != 8 || screen->uncorrectedRGBFormat.bshift != 0))
        reorderPixelBytes(screen, 16, 8, 0, 24);
    else if(screen->bitplaneDepth == 32 && (screen->uncorrectedRGBFormat.rshift != 24 || screen->uncorrectedRGBFormat.gshift != 16 || screen->uncorrectedRGBFormat.bshift != 8 || screen->uncorrectedRGBFormat.ashift != 0))
        reorderPixelBytes(screen, 24, 16, 8, 0);
}
