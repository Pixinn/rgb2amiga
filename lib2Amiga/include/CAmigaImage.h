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

#ifndef CAMIGAIMAGE_H
#define CAMIGAIMAGE_H

#include "CChunkyImage.h"

class CChunkyImage;
struct amiVideo_Screen;

class CAmigaImage
{
public:
    CAmigaImage();
    ~CAmigaImage(void);

    void Init(CChunkyImage&);

    void Save(const std::string & filepath);

private:
    static const unsigned int BITPLANE_DEPTH_MAX = 6;
    static const unsigned int OCS_COLORS_PER_CHANNEL = 4;

    struct
    {
        short width;
        short height;
        unsigned int bitplaneDepth;
        int viewportMode;
        uint8_t* bitplanes[BITPLANE_DEPTH_MAX];
    } _viewport;
    
    amiVideo_Screen* _screen = nullptr; //Screen conversion structure
    uint8_t* _pBitplanes = nullptr;     //Memory to store bitplanes
    bool _isInitialized = false;
};

#endif // CAMIGAIMAGE_H
