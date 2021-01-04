#ifndef __AMIVIDEO_SCREEN_H
#define __AMIVIDEO_SCREEN_H

#define AMIVIDEO_MAX_NUM_OF_BITPLANES 32

#include "amivideotypes.h"
#include "palette.h"

#ifdef __cplusplus
extern "C"{
#endif 

typedef struct amiVideo_Screen amiVideo_Screen;

/**
 * A data structure representing an Amiga screen (or viewport) containing
 * conversion sub structures that store the screen in a different displaying
 * format.
 */
struct amiVideo_Screen
{
    /** Defines the width of the screen in pixels */
    amiVideo_Word width;
    
    /** Defines the height of the screen in pixels */
    amiVideo_Word height;
    
    /** Defines the bitplane depth */
    unsigned int bitplaneDepth;
    
    /** Contains the viewport mode settings */
    amiVideo_Long viewportMode;
    
    /* Contains the values of the color registers and its converted values */
    amiVideo_Palette palette;
    
    /**
     * Contains all the relevant properties of the current screen to display it
     * in planar format -- the format that Amiga's OCS, ECS and AGA chipsets use.
     * In this format, each bit represents a part of the palette index of a pixel
     * and is stored the amount bitplanes times in memory.
     *
     * For example, a screen with 32 colors has a bitplane depth of 5. In this
     * case the pixels are stored five times in memory in which each bit of a
     * plane represents a pixel.
     * 
     * The bits in first occurence of the bitplane represent the most significant
     * bit and the last the least significant bit of the pixel's index value.
     * By combining all these bits from the most significant bit to the least
     * significant bit we will get the index value of the palette of the pixel.
     *
     * Widths are padded till the nearest word boundary (i.e. multiples of 16).
     */
    struct
    {
	/** Contains pointers to each bitplane section that stores parts of each pixel's color component */
	amiVideo_UByte *bitplanes[AMIVIDEO_MAX_NUM_OF_BITPLANES];
	
	/** Contains the padded width in pixels that is rounded up to the nearest word boundary */
	unsigned int pitch;
	
	/** Indicates whether the pixel memory is allocated and needs to be freed */
	int memoryAllocated;
    }
    bitplaneFormat;
    
    /**
     * Contains all the relevant properties of the current screen to display it
     * in chunky format -- used by PC displays with 256 colors. In this format,
     * each byte represents a pixel in which the value refers to an index in the
     * palette.
     *
     * The screen width may be padded.
     */
    struct
    {
	/** Contains the padded screen width in bytes */
	unsigned int pitch;
	
	/** Contains the pixel data in which each byte represents an index in the palette */
	amiVideo_UByte *pixels;
	
	/** Indicates whether the pixel memory is allocated and needs to be freed */
	int memoryAllocated;
    }
    uncorrectedChunkyFormat;
    
    /** 
     * Contains all the relevant properties of the current screen to display it
     * in RGB format in which every four bytes represent the red, green, blue
     * value of a pixel and a padding byte.
     *
     * The screen width may be padded.
     */
    struct
    {
	/** Contains the padded screen width in bytes (usually rounded up to the nearest 4-byte boundary) */
	unsigned int pitch;
	
	/** Contains the amount of bits that we have to left shift the red color component */
	amiVideo_UByte rshift;
	
	/** Contains the amount of bits that we have to left shift the green color component */
	amiVideo_UByte gshift;
	
	/** Contains the amount of bits that we have to left shift the blue color component */
	amiVideo_UByte bshift;
	
	/** Contains the amount of bits that we have to left shift the alpha color component */
	amiVideo_UByte ashift;
	
	/** Contains the pixel data in which each four bytes represent red, glue, blue values and a padding byte */
	amiVideo_ULong *pixels;
	
	/** Indicates whether the pixel memory is allocated and needs to be freed */
	int memoryAllocated;
    }
    uncorrectedRGBFormat;
    
    /**
     * Contains the screen in a format that has the correct aspect ratio, as
     * Amiga displays have only have a horizontal pixel resolution and interlace
     * mode doubling the amount of available scanlines.
     *
     * A lowres pixel is composed of two highres pixels, thus the width of a
     * lowres resolution screen must be scaled at least by a factor 2.
     */
    struct
    {
	/** Contains the width of the corrected screen */
	int width;
	
	/** Contains the height of the corrected screen */
	int height;
	
	/** Contains the padded width of the corrected screen (usually rounded up to the nearest 4-byte boundary) */
	unsigned int pitch;
	
	/** Contains the amount of bytes per pixel (1 = chunky, 4 = RGB) */
	unsigned int bytesPerPixel;
	
	/* Specifies the width of a lowres pixel in real pixels. Usually 2 is sufficient. To support super hires displays, 4 is required. */
	unsigned int lowresPixelScaleFactor;
	
	/** Contains the pixel data */
	void *pixels;
    }
    correctedFormat;
};

typedef enum
{
    AMIVIDEO_CHUNKY_FORMAT = 1,
    AMIVIDEO_RGB_FORMAT = 4
}
amiVideo_ColorFormat;

/**
 * Initializes a screen instance with the given dimensions, bitplane depth,
 * specific size of color components and viewport mode.
 *
 * @param screen Screen conversion structure
 * @param width Width of the screen in pixels
 * @param height Height of the screen in scanlines
 * @param bitplaneDepth Bitplane depth, a value between 1-6 (OCS/ECS) and 1-8 (AGA)
 * @param bitsPerColorChannel The amount of bits for used for a color component (4 = ECS/OCS, 8 = AGA)
 * @param viewportMode The viewport mode value
 */
void amiVideo_initScreen(amiVideo_Screen *screen, amiVideo_Word width, amiVideo_Word height, unsigned int bitplaneDepth, unsigned int bitsPerColorChannel, amiVideo_Long viewportMode);

/**
 * Creates a screen conversion structure on the heap with the given dimensions,
 * bitplane depth, specific size of color components and viewport mode. The
 * resulting screen must eventually be freed from memory by calling amiVideo_freeScreen()
 *
 * @param width Width of the screen in pixels
 * @param height Height of the screen in scanlines
 * @param bitplaneDepth Bitplane depth, a value between 1-6 (OCS/ECS) and 1-8 (AGA)
 * @param bitsPerColorChannel The amount of bits for used for a color component (4 = ECS/OCS, 8 = AGA)
 * @param viewportMode The viewport mode value
 * @return A screen conversion structure with the given properties
 */
amiVideo_Screen *amiVideo_createScreen(amiVideo_Word width, amiVideo_Word height, unsigned int bitplaneDepth, unsigned int bitsPerColorChannel, amiVideo_Long viewportMode);

/**
 * Frees the heap allocated members of the given screen structure.
 *
 * @param screen Screen conversion structure
 */
void amiVideo_cleanupScreen(amiVideo_Screen *screen);

/**
 * Frees the given screen conversion structure and its members.
 *
 * @param screen Screen conversion structure
 */
void amiVideo_freeScreen(amiVideo_Screen *screen);

/**
 * Calculates the width of a surface that has corrected the aspect ratio.
 *
 * @param lowresPixelScaleFactor Specifies the width of a lowres pixel in real pixels. Usually 2 is sufficient. To support super hires displays, 4 is required.
 * @param width Width of the uncorrected surface
 * @param viewportMode The viewport mode value
 */
int amiVideo_calculateCorrectedWidth(unsigned int lowresPixelScaleFactor, amiVideo_Long width, amiVideo_Long viewportMode);

/**
 * Calculates the height of a surface that has corrected the aspect ratio.
 *
 * @param lowresPixelScaleFactor Specifies the width of a lowres pixel in real pixels. Usually 2 is sufficient. To support super hires displays, 4 is required.
 * @param height Height of the uncorrected surface
 * @param viewportMode The viewport mode value
 */
int amiVideo_calculateCorrectedHeight(unsigned int lowresPixelScaleFactor, amiVideo_Long height, amiVideo_Long viewportMode);

/**
 * @param screen Screen conversion structure
 * @param lowresPixelScaleFactor Specifies the width of a lowres pixel in real pixels. Usually 2 is sufficient. To support super hires displays, 4 is required.
 */
void amiVideo_setLowresPixelScaleFactor(amiVideo_Screen *screen, unsigned int lowresPixelScaleFactor);

/**
 * Sets the bitplane pointers of the conversion structure to the appropriate
 * memory positions. On AmigaOS these may point to a real viewport's bitplane
 * pointers. On different platforms these may point to subsets of a
 * pre-allocated memory area containing planar graphics data.
 *
 * @param screen Screen conversion structure
 * @param bitplanes Pointers to bitplane areas in memory
 */
void amiVideo_setScreenBitplanePointers(amiVideo_Screen *screen, amiVideo_UByte **bitplanes);

/**
 * Automatically sets the bitplane pointers of the conversion structure to the
 * right subsets in a given memory area containing planar graphics data. It
 * assumes that planar data for each bitplane level are stored immediately after
 * each other.
 *
 * @param screen Screen conversion structure
 * @param bitplanes A memory area containing planar graphics data
 */
void amiVideo_setScreenBitplanes(amiVideo_Screen *screen, amiVideo_UByte *bitplanes);

/**
 * Sets the uncorrected chunky sub struct pointer to a memory area capable of
 * storing it.
 *
 * @param screen Screen conversion structure
 * @param pixels Pointer to a memory struct storing chunky pixels
 * @param pitch Padded width of the memory surface in bytes
 */
void amiVideo_setScreenUncorrectedChunkyPixelsPointer(amiVideo_Screen *screen, amiVideo_UByte *pixels, unsigned int pitch);

/**
 * Sets the uncorrected RGB sub struct pointer to a memory area capable of
 * storing it.
 *
 * @param screen Screen conversion structure
 * @param pixels Pointer to a memory area storing RGB pixels
 * @param pitch Padded width of the memory surface in bytes (usually 4 * width, but it may be padded)
 * @param allocateUncorrectedMemory Indicates whether we should allocate memory for a chunky pixels buffer that should be freed
 * @param rshift The amount of bits that we have to left shift the red color component
 * @param gshift The amount of bits that we have to left shift the green color component
 * @param bshift The amount of bits that we have to left shift the blue color component
 * @param ashift The amount of bits that we have to left shift the alpha color component
 */
void amiVideo_setScreenUncorrectedRGBPixelsPointer(amiVideo_Screen *screen, amiVideo_ULong *pixels, unsigned int pitch, int allocateUncorrectedMemory, amiVideo_UByte rshift, amiVideo_UByte gshift, amiVideo_UByte bshift, amiVideo_UByte ashift);

/**
 * Sets the corrected pixels sub struct pointer to a memory area capable of
 * storing it.
 *
 * @param screen Screen conversion structure
 * @param pixels Pointer to a memory area storing the corrected pixels
 * @param pitch Padded width of the memory surface in bytes (equals witdth for chunky, 4 * width for RGB, but it may be padded)
 * @param bytesPerPixel Specifies of how many bytes a pixel consists (1 = chunky, 4 = RGB)
 * @param allocateUncorrectedMemory Indicates whether we should allocate memory for a chunky or RGB pixel buffer that should be freed
 * @param rshift The amount of bits that we have to left shift the red color component
 * @param gshift The amount of bits that we have to left shift the green color component
 * @param bshift The amount of bits that we have to left shift the blue color component
 * @param ashift The amount of bits that we have to left shift the alpha color component
 */
void amiVideo_setScreenCorrectedPixelsPointer(amiVideo_Screen *screen, void *pixels, unsigned int pitch, unsigned int bytesPerPixel, int allocateUncorrectedMemory, amiVideo_UByte rshift, amiVideo_UByte gshift, amiVideo_UByte bshift, amiVideo_UByte ashift);

/**
 * Converts the bitplanes to chunky pixels in which every byte represents a
 * pixel and an index value from the palette.
 *
 * @param screen Screen conversion structure
 */
void amiVideo_convertScreenBitplanesToChunkyPixels(amiVideo_Screen *screen);

/**
 * Converts the chunky pixels to RGB pixels in which every four bytes represent
 * the color value of a pixel.
 *
 * @param screen Screen conversion structure
 */
void amiVideo_convertScreenChunkyPixelsToRGBPixels(amiVideo_Screen *screen);

/**
 * Converts chunky pixels to bitplane format in which every bit represents a
 * part of an index value of the palette of a pixel.
 *
 * @param screen Screen conversion structure
 */
void amiVideo_convertScreenChunkyPixelsToBitplanes(amiVideo_Screen *screen);

/**
 * Corrects the chunky or RGB pixel surface into a surface having the correct
 * aspect ratio taking the resolution settings into account.
 *
 * @param screen Screen conversion structure
 */
void amiVideo_correctScreenPixels(amiVideo_Screen *screen);

/**
 * Converts the screen bitplane surface to RGB pixel surface and performs all
 * the immediate steps.
 *
 * @param screen Screen conversion structure
 */
void amiVideo_convertScreenBitplanesToRGBPixels(amiVideo_Screen *screen);

/**
 * Converts the screen bitplanes surface to a corrected chunky pixel surface and
 * performs all the immediate steps.
 *
 * @param screen Screen conversion structure
 */
void amiVideo_convertScreenBitplanesToCorrectedChunkyPixels(amiVideo_Screen *screen);

/**
 * Converts the screen bitplanes surface to a corrected RGB pixel surface and
 * performs all the immediate steps.
 *
 * @param screen Screen conversion structure
 */
void amiVideo_convertScreenBitplanesToCorrectedRGBPixels(amiVideo_Screen *screen);

/**
 * Converts the uncorrected chunky pixel surface to a corrected RGB pixel surface
 * and performs all the immediate steps.
 *
 * @param screen Screen conversion structure
 */
void amiVideo_convertScreenChunkyPixelsToCorrectedRGBPixels(amiVideo_Screen *screen);

/**
 * Auto selects the most efficient display format for displaying the converted
 * screen. It picks RGB format for HAM displays and when 24 or 32 bitplanes are
 * used. In all other cases, it picks chunky format.
 *
 * @param viewportMode Amiga viewport register value
 * @return The most efficient color format
 */
amiVideo_ColorFormat amiVideo_autoSelectColorFormat(const amiVideo_Screen *screen);

/**
 * Reorders the RGB pixels from the 0RGB (for 24 bitplanes) or RGBA (for 32
 * bitplanes) representation to the byte order that is actually used for the
 * display screen.
 *
 * @param screen Screen conversion structure
 */
void amiVideo_reorderRGBPixels(amiVideo_Screen *screen);

#ifdef __cplusplus
}
#endif

#endif
