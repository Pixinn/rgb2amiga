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
struct rgba8Bits_t
{
 
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  uint8_t a = 0xff;

  static constexpr double LUMA_RED = 0.299;
  static constexpr double LUMA_GREEN = 0.587;
  static constexpr double LUMA_BLUE = 0.114;

  rgba8Bits_t(void)
  {    }

  template<typename T>
  rgba8Bits_t(const T red, const T green, const T blue, const T alpha = std::numeric_limits<T>::max())
  {    
    constexpr unsigned sz_quantum = sizeof(T);
    constexpr unsigned rounder = sz_quantum > 1 ? 0x80 << (sz_quantum - 2) : 0;
    constexpr unsigned shift = 8 * (sz_quantum - 1);
    unsigned round = red < (1 << 8*sz_quantum) - rounder ? rounder : 0;
    r = ((red + round) >> shift) & 0xFF;
    round = green < (1 << 8*sz_quantum) - rounder ? rounder : 0;
    g = ((green + round) >> shift) & 0xFF;
    round = blue < (1 << 8*sz_quantum) - rounder ? rounder : 0;
    b = ((blue + round) >> shift) & 0xFF;
    round = alpha < (1 << 8 * sz_quantum) - rounder ? rounder : 0;
    a = ((alpha + round) >> shift) & 0xFF;
  }

  inline unsigned int Hash(void) const {
      return static_cast < unsigned int > ( (r << 16) | (g << 8) | b );
  }

  inline double Distance(const rgba8Bits_t& color ) const
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
  }


  inline bool operator== (const rgba8Bits_t& other) const {
      return (r == other.r) && (g == other.g) && (b == other.b);
  }

  inline bool operator<(const rgba8Bits_t& other) const { //Returns true if this' luma < other's luma
      return((LUMA_RED * r + LUMA_GREEN * g + LUMA_BLUE * b) < (LUMA_RED * other.r + LUMA_GREEN * other.g + LUMA_BLUE * other.b));
  }
} ;



/******************************/
/*       CLASS CPALETTE       */
/******************************/
class CPalette : public std::vector<rgba8Bits_t>
{
public:
    CPalette( void )
    {}
    CPalette(const unordered_map < unsigned int, rgba8Bits_t >& colors);
    CPalette(const std::vector<rgba8Bits_t >& colors);

    void Save(const string& filename) const;

    rgba8Bits_t GetNearestColor(const rgba8Bits_t& color) const; //Returns the nearest color in the palette 
    
private:

  /// @brief Sorting colors in palette to help editing width Deluxe Paint
  ///  @details PLUS: having black as colors 0 appears to help color fidelity on the *real* hardware
  void Sort();
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
    inline rgba8Bits_t Get8bitRGB(uint8_t red, uint8_t green, uint8_t blue) {
      rgba8Bits_t color;
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
