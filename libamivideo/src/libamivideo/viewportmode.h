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

#ifndef __AMIVIDEO_VIEWPORTMODE_H
#define __AMIVIDEO_VIEWPORTMODE_H
#include "amivideotypes.h"

#define AMIVIDEO_VIDEOPORTMODE_LACE 0x0004
#define AMIVIDEO_VIDEOPORTMODE_EHB 0x80
#define AMIVIDEO_VIDEOPORTMODE_HAM 0x800
#define AMIVIDEO_VIDEOPORTMODE_HIRES 0x8000
#define AMIVIDEO_VIDEOPORTMODE_SUPERHIRES 0x8020

#ifdef __cplusplus
extern "C"{
#endif 

    /**
     * Checks whether the Extra-Halfbrite (EHB) bit is enabled in the viewport mode register.
     *
     * @param viewportMode Amiga viewport register value
     * @return TRUE if Extra-Halfbrite is enabled, else FALSE
     */
    int amiVideo_checkExtraHalfbrite(const amiVideo_Long viewportMode);

    /**
     * Checks whether the Hold-and-Modify (HAM) bit is enabled in the viewport mode register.
     *
     * @param viewportMode Amiga viewport register value
     * @return TRUE if Extra-Halfbrite is enabled, else FALSE
     */
    int amiVideo_checkHoldAndModify(const amiVideo_Long viewportMode);

    /**
     * Checks whether the hires bit is enabled in the viewport mode register.
     *
     * @param viewportMode Amiga viewport register value
     * @return TRUE if the hires bit is enabled, else FALSE
     */
    int amiVideo_checkHires(const amiVideo_Long viewportMode);

    /**
     * Checks whether the super and hires bits are enabled in the viewport mode
     * register.
     *
     * @param viewportMode Amiga viewport register value
     * @return TRUE if the hires and super bits are enabled, else FALSE
     */
    int amiVideo_checkSuperHires(const amiVideo_Long viewportMode);

    /**
     * Checks whether the hires bit is enabled in the viewport mode register.
     *
     * @param viewportMode Amiga viewport register value
     * @return TRUE if the hires bit is enabled, else FALSE
     */
    int amiVideo_checkLaced(const amiVideo_Long viewportMode);

    /**
     * Auto selects the most space efficient lowres pixel scale factor capable of
     * retaining the right aspect ratio on non-Amiga displays.
     *
     * @param viewportMode Amiga viewport register value
     * @return The most efficient lowres pixel scale factor
     */
    unsigned int amiVideo_autoSelectLowresPixelScaleFactor(const amiVideo_Long viewportMode);

    /**
     * Extracts the palette flag values bits (Extra Half Brite and Hold-and Modify)
     * from the viewport mode value.
     *
     * @param viewportMode Amiga viewport register value
     * @return A viewport mode value with only the EHB and HAM flags set
     */
    amiVideo_Long amiVideo_extractPaletteFlags(const amiVideo_Long viewportMode);

    /**
     * Auto selects the most suitable Amiga resolution viewport flags to display a
     * given screen.
     *
     * @param width Width of the screen
     * @param height Height of the screen
     * @return A viewport mode value with the most suitable resolution flags set
     */
    amiVideo_Long amiVideo_autoSelectViewportMode(const amiVideo_Word width, const amiVideo_Word height);

#ifdef __cplusplus
}
#endif 

#endif
