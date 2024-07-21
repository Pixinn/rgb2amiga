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

#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <iostream>

#include "CPalette.h"

//statics
CPaletteFactory* CPaletteFactory::_Instance = nullptr;

//+++++++++ CPALETTE ++++++++++++//
CPalette::CPalette(const unordered_map<unsigned int, rgba8Bits_t>& colors)
{
    for (auto color : colors) {
        push_back(color.second);
    }
    Sort();
}
CPalette::CPalette(const std::vector<rgba8Bits_t >& colors)
  : std::vector<rgba8Bits_t>{ colors }
{
  Sort();
}



rgba8Bits_t CPalette::GetNearestColor(const rgba8Bits_t& color) const
{
    auto minDistance = std::numeric_limits<double>::max();
    rgba8Bits_t colorFound;
    for (auto colorInPalette : *this)
    {
        const auto newDistance = colorInPalette.Distance(color);
        if (newDistance < minDistance) {
            minDistance = newDistance;
            colorFound = colorInPalette;
        }
    }
    return colorFound;
}

void CPalette::Sort()
{
  std::sort(this->begin(), this->end());
  auto pLastColor = this->end() - 1;
  auto lightest = *pLastColor;
  *pLastColor = (*this)[1];
  (*this)[1] = lightest;
}

void CPalette::Save(const string& filename) const
{
  ofstream outfile;
  outfile.open(filename, ios::out | ios::trunc | ios::binary);
  outfile << "GIMP Palette\n";
  outfile << "Name: " << filename << "\n";
  outfile << "Columns: 16\n";
  outfile << "#\n";
  outfile << std::dec;
  for (auto color : *this) {
    outfile << unsigned(color.r) << " " << unsigned(color.g) << " " << unsigned(color.b) << "\n";
  }
  outfile.close();
}

//+++++++++ CPALETTEFACTORY ++++++++++++//
CPaletteFactory::CPaletteFactory(void)
{
    GenerateAmigaReduced(); 
    GenerateAmiga();
}


void CPaletteFactory::GenerateAmiga()
{
    uint8_t amigaChannelMax = 15;
    for (auto red = 0; red <= amigaChannelMax; red++)  {
        for (auto green = 0; green <= amigaChannelMax; green++) {
            for (auto blue = 0; blue <= amigaChannelMax; blue++) {
                _amiga.push_back(Get8bitRGB(red, green, blue));
            }
        }
    }
}

void CPaletteFactory::GenerateAmigaReduced()
{
    _amiga_reduced.push_back( Get8bitRGB(0x0, 0x0, 0x0) );
    _amiga_reduced.push_back( Get8bitRGB(0xA, 0xA, 0xA) );
    _amiga_reduced.push_back( Get8bitRGB(0xE, 0x0, 0x0) );
    _amiga_reduced.push_back( Get8bitRGB(0xA, 0x0, 0x0) );
    _amiga_reduced.push_back( Get8bitRGB(0xD, 0x8, 0x0) );
    _amiga_reduced.push_back( Get8bitRGB(0xF, 0xE, 0x0) );
    _amiga_reduced.push_back( Get8bitRGB(0x8, 0xF, 0x0) );
    _amiga_reduced.push_back( Get8bitRGB(0x0, 0x8, 0x0) );
    _amiga_reduced.push_back( Get8bitRGB(0x0, 0xB, 0x0) );
    _amiga_reduced.push_back( Get8bitRGB(0x0, 0xD, 0xD) );
    _amiga_reduced.push_back( Get8bitRGB(0x0, 0xA, 0xF) );
    _amiga_reduced.push_back( Get8bitRGB(0x0, 0x7, 0xC) );
    _amiga_reduced.push_back( Get8bitRGB(0x0, 0x0, 0xF) );
    _amiga_reduced.push_back( Get8bitRGB(0x7, 0x0, 0xF) );
    _amiga_reduced.push_back( Get8bitRGB(0xC, 0x0, 0xE) );
    _amiga_reduced.push_back( Get8bitRGB(0xC, 0x0, 0x8) );
    _amiga_reduced.push_back( Get8bitRGB(0x6, 0x2, 0x0) );
    _amiga_reduced.push_back( Get8bitRGB(0xE, 0x5, 0x2) );
    _amiga_reduced.push_back( Get8bitRGB(0xA, 0x5, 0x2) );
    _amiga_reduced.push_back( Get8bitRGB(0xF, 0xC, 0xA) );
    _amiga_reduced.push_back( Get8bitRGB(0x3, 0x3, 0x3) );
    _amiga_reduced.push_back( Get8bitRGB(0x4, 0x4, 0x4) );
    _amiga_reduced.push_back( Get8bitRGB(0x5, 0x5, 0x5) );
    _amiga_reduced.push_back( Get8bitRGB(0x6, 0x6, 0x6) );
    _amiga_reduced.push_back( Get8bitRGB(0x7, 0x7, 0x7) );
    _amiga_reduced.push_back( Get8bitRGB(0x8, 0x8, 0x8) );
    _amiga_reduced.push_back( Get8bitRGB(0x9, 0x9, 0x9) );
    _amiga_reduced.push_back( Get8bitRGB(0xA, 0xA, 0xA) );
    _amiga_reduced.push_back( Get8bitRGB(0xC, 0xC, 0xC) );
    _amiga_reduced.push_back( Get8bitRGB(0xD, 0xD, 0xD) );
    _amiga_reduced.push_back( Get8bitRGB(0xE, 0xE, 0xE) );
    _amiga_reduced.push_back( Get8bitRGB(0xF, 0xF, 0xF) );
}

const CPalette& CPaletteFactory::GetPalette(const string& key ) const
{
    if (key == "AMIGA_REDUCED") {
        return _amiga_reduced;
    }
    return _amiga;
}


CPalette CPaletteFactory::GetUniqueColors(Magick::Image& image) const
{
    std::unordered_map<unsigned int, rgba8Bits_t> uniqueColors;
    PixelPacket* pixel = image.getPixels(0,0,image.size().width(), image.size().height());
    
    for (unsigned int i = 0; i < image.size().width() * image.size().height(); i++)
    {
        rgba8Bits_t color{ pixel->red, pixel->green, pixel->blue };
        ++pixel;

        //Add color if not present in unique colors colection
        auto search = uniqueColors.find(color.Hash());
        if (search == uniqueColors.end()) {
            uniqueColors.insert({ color.Hash(), color });
        }
    }

    return CPalette{ uniqueColors };
}


//Build and returns a new palette by finding the closest colors in the space
CPalette CPaletteFactory::MapPalette(const CPalette& palette, const CPalette& space) const
{
    CPalette nearestColors;
    
    for (const auto& color : palette) {
        nearestColors.push_back( space.GetNearestColor(color) );
    }

    return nearestColors;
}

