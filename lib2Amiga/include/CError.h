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



#ifndef CERROR_H
#define CERROR_H

#include <string>

class CError
{
public:
    CError(const std::string& what) :
        _what(what)
    {    }
    inline const std::string& What(void) const { return _what; }

private:
    std::string _what;
};

#endif // CERROR_H
