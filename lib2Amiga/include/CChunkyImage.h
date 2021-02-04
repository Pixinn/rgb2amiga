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

#ifndef CPLANARIMAGE_H
#define CPLANARIMAGE_H


#include "CPalette.h"
#include "CAmigaImage.h"

#include <Magick++.h>


class CChunkyImage
{
public:
    void Init(const Magick::Image&, const unsigned int nbColors, const bool dither, const CPalette&, const string& size);

    inline bool IsInitialized() const { return _isInitialized; }

    inline int GetWidth(void) const { return static_cast<int>(_imageRGB.size().width()); }
    inline int GetHeight(void) const { return static_cast<int>(_imageRGB.size().height()); }

    inline const std::vector< uint8_t >& GetPixels(void) const { return _imageIdx; }
    inline const CPalette&  GetPalette(void) const { return _palette; }

private:
    void Map() //Maps the colors of_image those of _palette
    {}

    Magick::Image _imageRGB;
    std::vector< uint8_t > _imageIdx;
    CPalette _palette;
    bool _isInitialized = false;

    static const unsigned int OCS_MAX_COLORS = 32;
};


#endif // CPLANARIMAGE_H
