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

#ifndef __AMIVIDEO_PALETTE_H
#define __AMIVIDEO_PALETTE_H
#include "amivideotypes.h"


#ifdef __cplusplus
extern "C"{
#endif 

    /**
     * @brief Struct storing values of a color channel.
     */
    typedef struct
    {
        /** Defines the intensity of the red color channel */
        amiVideo_UByte r;

        /** Defines the intensity of the green color channel */
        amiVideo_UByte g;

        /** Defines the intensity of the blue color channel */
        amiVideo_UByte b;
    }
    amiVideo_Color;

    /**
     * @brief Struct storing values of a color channel.
     * This struct type has the same structure as the SDL_Color struct.
     */
    typedef struct
    {
        /** Defines the intensity of the red color channel */
        amiVideo_UByte r;

        /** Defines the intensity of the green color channel */
        amiVideo_UByte g;

        /** Defines the intensity of the blue color channel */
        amiVideo_UByte b;

        /** Defines the intensity of the alpha color channel */
        amiVideo_UByte a;
    }
    amiVideo_OutputColor;

    typedef struct
    {
        struct
        {
            /** Contains the viewport mode settings */
            amiVideo_Long viewportMode;

            /** Contains the number of colors in the original Amiga palette */
            unsigned int numOfColors;

            /** Stores the color values of the palette */
            amiVideo_Color *color;

            /** Contains the number of bits that a color component has (4 = OCS/ECS, 8 = AGA) */
            unsigned int bitsPerColorChannel;
        }
        bitplaneFormat;

        struct
        {
            /** Contains the number of colors in the converted chunky graphics palette */
            unsigned int numOfColors;

            /** Stores the color values of the palette */
            amiVideo_OutputColor *color;
        }
        chunkyFormat;
    }
    amiVideo_Palette;

    /**
     * Initialises the palette with the given bitplane depth, bits per color channel
     * and viewport mode.
     *
     * @param palette Palette conversion structure
     * @param bitplaneDepth Bitplane depth, a value between 1-6 (OCS/ECS) and 1-8 (AGA)
     * @param bitsPerColorChannel The amount of bits for used for a color component (4 = ECS/OCS, 8 = AGA)
     * @param viewportMode The viewport mode value
     */
    void amiVideo_initPalette(amiVideo_Palette *palette, unsigned int bitplaneDepth, unsigned int bitsPerColorChannel, amiVideo_Long viewportMode);

    /**
     * Frees all the heap allocated members of the palette from memory.
     *
     * @param palette Palette conversion structure
     */
    void amiVideo_cleanupPalette(amiVideo_Palette *palette);

    /**
     * Sets the palette's bitplane color values to the values in a given array. The
     * remaining colors are set to 0. The color values must be in the Amiga screen's
     * format, i.e. 4 bits or 8 bits per pixel.
     *
     * @param palette Palette conversion structure
     * @param color Array of color values
     * @param numOfColors The amount of colors in the color value array
     */
    void amiVideo_setBitplanePaletteColors(amiVideo_Palette *palette, amiVideo_Color *color, unsigned int numOfColors);

    /**
     * Sets the palette's chunky color values to the values in a given array. The
     * remaining colors are set to 0. The color values must be in the chunky screen
     * format, i.e. 8 bits per pixel.
     *
     * @param palette Palette conversion structure
     * @param color Array of color values
     * @param numOfColors The amount of colors in the color value array
     */
    void amiVideo_setChunkyPaletteColors(amiVideo_Palette *palette, amiVideo_OutputColor *color, unsigned int numOfColors);

    /**
     * Converts the original palette used for the bitplanes to the ones used for
     * displaying chunky graphics. This means that a palette in which the color
     * components consist 4 bits are converted to 8 bits.
     *
     * If the viewportMode has the extra halfbrite bit set, then the amount of
     * colors are doubled in which the color values of latter half, are half of the
     * values of the first half of the palette.
     *
     * @param palette Palette conversion structure
     */
    void amiVideo_convertBitplaneColorsToChunkyFormat(amiVideo_Palette *palette);

    /**
     * Converts the palette used for chunky graphics to a format that can be for
     * displaying bitplanes. If an palette with 4 bit color components is used,
     * then the color components are shifted.
     *
     * @param palette Palette conversion structure
     */
    void amiVideo_convertChunkyColorsToBitplaneFormat(amiVideo_Palette *palette);

    /**
     * Converts the bitplane palette to an array of word specifications, which can
     * be used by AmigaOS' LoadRGB4() function to set a screen's palette. This
     * function does not support the AGA chipset's capabilities. The resulting array
     * has as many elements as the bitplane palette and is allocated on the heap.
     * Therefore, free() must be invoked when it has become obsolete.
     *
     * @param palette Palette conversion structure
     * @return A word array containing the color specifications for the LoadRGB4() function.
     */
    amiVideo_UWord *amiVideo_generateRGB4ColorSpecs(const amiVideo_Palette *palette);

    /**
     * Converts the bitplane palette to an array of long integer color
     * specifications, which can be used by AmigaOS' LoadRGB32() function to set a
     * screen's palette. To use the AGA chipset's abilities, it's required to use
     * this function. The resulting array is allocated on the heap and must be
     * deleted with free() when it has become obsolete.
     *
     * @param palette Palette conversion structure
     * @return An long integer array containing the color specifications for the LoadRGB32() function.
     */
    amiVideo_ULong *amiVideo_generateRGB32ColorSpecs(const amiVideo_Palette *palette);

#ifdef __cplusplus
}
#endif 

#endif
