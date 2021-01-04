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



#ifndef CPALETTE_H
#define CPALETTE_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <Magick++.h>


using namespace Magick;
using namespace std;



//MUST BE BIT EXACT BITH "amiVideo_Color"
struct rgb8Bits_t{
 
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  uint8_t padding = 0;

  static constexpr double LUMA_RED = 0.299;
  static constexpr double LUMA_GREEN = 0.587;
  static constexpr double LUMA_BLUE = 0.114;

  rgb8Bits_t(void)
  {    }

  template<typename T>
  rgb8Bits_t(const T red, const T green, const T blue)
  {    
    const unsigned sz_quantum = sizeof(T);
    const unsigned rounder = sz_quantum > 1 ? 0x80 << (sz_quantum - 2) : 0;
    const unsigned shift = 8 * (sz_quantum - 1);
    r = ((red + rounder) >> shift) & 0xFF;
    g = ((green + rounder) >> shift) & 0xFF;
    b = ((blue + rounder) >> shift) & 0xFF;  
  }

  inline unsigned int Hash(void) const {
      return static_cast < unsigned int > ( (r << 16) | (g << 8) | b );
  }

  inline double Distance(const rgb8Bits_t& color ) const
  {
    const auto y1 = LUMA_RED * r + LUMA_GREEN * g + LUMA_BLUE * b;
    const auto u1 = 0.492 * (b - y1);
    const auto v1 = 0.877 * (r - y1);
    const auto y2 = LUMA_RED * color.r + LUMA_GREEN * color.g + LUMA_BLUE * color.b;
    const auto u2 = 0.492 * (color.b - y2);
    const auto v2 = 0.877 * (color.r - y2);

    const auto dy = (y1 - y2);
    const auto du = (u1 - u2);
    const auto dv = (v1 - v2);

    return (dy * dy + du * du + dv * dv);

      //int dr = (r - color.r);
      //int dg = (g - color.g);
      //int db = (b - color.b);
      //return (dr*dr + dg*dg + db*db);
  }

  inline bool operator== (const rgb8Bits_t& other) const {
      return (r == other.r) && (g == other.g) && (b == other.b);
  }

  inline bool operator<(const rgb8Bits_t& other) const { //Returns true if this' luma < other's luma
      return((LUMA_RED * r + LUMA_GREEN * g + LUMA_BLUE * b) < (LUMA_RED * other.r + LUMA_GREEN * other.g + LUMA_BLUE * other.b));
  }
} ;



/******************************/
/*       CLASS CPALETTE       */
/******************************/
class CPalette
{
public:
    CPalette( void )
    {}
    CPalette(const unordered_map < unsigned int, rgb8Bits_t >& colors);

    rgb8Bits_t NearestColor(const rgb8Bits_t& color) const; //Returns the nearest color in the palette

    inline const vector < rgb8Bits_t >& GetColors(void) const {
        return _data;
    }
    
    //Facades to _data
    inline void PushBack(const rgb8Bits_t& color) {
        _data.push_back(color);
    }
    inline std::size_t Size(void) const {
        return _data.size();
    }
    inline rgb8Bits_t* Data(void) {
        return _data.data();
    }


private:
    vector < rgb8Bits_t > _data;
};



/******************************/
/*   CLASS CPALETTEFACTORY    */
/******************************/
class CPaletteFactory
{
public:
    static CPaletteFactory& GetInstance(void) 
    {
        if (_Instance == nullptr) {
            _Instance = new CPaletteFactory();
        }
        return *_Instance;
    }

    ~CPaletteFactory() {
      delete _Instance;
    }

    const CPalette& GetPalette(const string& key) const;
    CPalette MapPalette(const CPalette& palette, const CPalette& space) const; //Maps the colors of the first palette to the space and returns the palette
    CPalette GetUniqueColors(Magick::Image&) const;

private:
    CPaletteFactory(void);

    void GenerateAmigaReduced(void);
    void GenerateAmiga(void);
    //Converts 4 bit color triplet to 8 bit color
    inline rgb8Bits_t Get8bitRGB(uint8_t red, uint8_t green, uint8_t blue) {
        rgb8Bits_t color;
        color.r = red * 17;
        color.g = green * 17;
        color.b = blue * 17;
        return color;
    }

    static CPaletteFactory* _Instance;

    CPalette _amiga;
    CPalette _amiga_reduced;
};

#endif // CPALETTE_H
