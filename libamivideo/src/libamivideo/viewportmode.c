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

#include "viewportmode.h"

int amiVideo_checkExtraHalfbrite(const amiVideo_Long viewportMode)
{
    return ((viewportMode & AMIVIDEO_VIDEOPORTMODE_EHB) == AMIVIDEO_VIDEOPORTMODE_EHB);
}

int amiVideo_checkHoldAndModify(const amiVideo_Long viewportMode)
{
    return ((viewportMode & AMIVIDEO_VIDEOPORTMODE_HAM) == AMIVIDEO_VIDEOPORTMODE_HAM);
}

int amiVideo_checkHires(const amiVideo_Long viewportMode)
{
    return ((viewportMode & AMIVIDEO_VIDEOPORTMODE_HIRES) == AMIVIDEO_VIDEOPORTMODE_HIRES);
}

int amiVideo_checkSuperHires(const amiVideo_Long viewportMode)
{
    return ((viewportMode & AMIVIDEO_VIDEOPORTMODE_SUPERHIRES) == AMIVIDEO_VIDEOPORTMODE_SUPERHIRES);
}

int amiVideo_checkLaced(const amiVideo_Long viewportMode)
{
    return ((viewportMode & AMIVIDEO_VIDEOPORTMODE_LACE) == AMIVIDEO_VIDEOPORTMODE_LACE);
}

unsigned int amiVideo_autoSelectLowresPixelScaleFactor(const amiVideo_Long viewportMode)
{
    if(amiVideo_checkSuperHires(viewportMode))
	return 4;
    else if(amiVideo_checkHires(viewportMode) && amiVideo_checkLaced(viewportMode))
	return 1;
    else if(amiVideo_checkHires(viewportMode) && !amiVideo_checkLaced(viewportMode))
	return 2;
    else if(amiVideo_checkLaced(viewportMode))
	return 2;
    else
	return 1;
}

amiVideo_Long amiVideo_extractPaletteFlags(const amiVideo_Long viewportMode)
{
    return viewportMode & (AMIVIDEO_VIDEOPORTMODE_HAM | AMIVIDEO_VIDEOPORTMODE_EHB);
}

amiVideo_Long amiVideo_autoSelectViewportMode(const amiVideo_Word width, const amiVideo_Word height)
{
    amiVideo_Long viewportMode = 0;
    
    if(width > 736)
	viewportMode |= AMIVIDEO_VIDEOPORTMODE_SUPERHIRES; /* If the page width is larger than 736 (640 width + max overscan), we use super hi-res screen mode */
    else if(width > 368)
	viewportMode |= AMIVIDEO_VIDEOPORTMODE_HIRES; /* If the page width is larger than 368 (320 width + max overscan), we use hi-res screen mode */

    /* If the page height is larger than 290 (256 height + max overscan), we have a laced screen mode */
    if(height > 290)
	viewportMode |= AMIVIDEO_VIDEOPORTMODE_LACE;

    return viewportMode;
}
