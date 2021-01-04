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


#include <string>
#include <Magick++.h> 
#include <iostream> 
#include "tclap/CmdLine.h"

#include "CError.h"

#include "CChunkyImage.h"
#include "CAmigaImage.h"

using namespace std;
using namespace Magick;


//**********************************//
// Required arguments:
// -i : input image
// -o : output file
//*********************************//
int main(int argc, char *argv[])
{
    try
    {
        //Parsing arguments
        TCLAP::CmdLine cmd{ "rgb2amiga - by Christophe Meneboeuf <christophe@xtof.info>", ' ', "1" };
        TCLAP::ValueArg<string> argInput{ "i", "input", "Input file to process.", true, "", "string" };
        TCLAP::ValueArg<string> argOutput("o", "output", "Output file.", true, "", "string");
        TCLAP::ValueArg<int>    argNbColors("c", "colors", "Number of colors to use. Defaults to \"32\".", false, 32, "string");
        TCLAP::ValueArg<string> argDither("d", "dither", "Use dithering. Set to <true> or <false>. Defaults to false.", false, "false", "string");
        TCLAP::ValueArg<string> argSize("s", "size", "Targeted size in WidthxHeight format. Defaults to \"320x256\"\n\tOptionnal suffix: '!' ignore the original aspect ratio.", false, "320x256", "string");
        
        //TCLAP::ValueArg<std::string> argPalette("p", "palette", "Palette to use.", true, "", "string");
        cmd.add(argInput);
        cmd.add(argOutput);
        cmd.add(argNbColors);
        cmd.add(argDither);
        cmd.add(argSize);
        cmd.parse( argc, argv );

        //Loading image 
        Image imgInput( argInput.getValue() );
        
        //Converting
        bool dithering = (argDither.getValue() != "false");
        CPalette palette = CPaletteFactory::GetInstance().GetPalette("AMIGA");
        CChunkyImage chunkyImg;
        chunkyImg.Init(imgInput, argNbColors.getValue(), dithering, palette, argSize.getValue());
        CAmigaImage amigaImg;
        amigaImg.Init(chunkyImg);
        amigaImg.Save(argOutput.getValue());
    }
    catch (TCLAP::ArgException &e)  // catch any exceptions
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        return 1;
    }
    catch (Magick::Exception &e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }
    catch (CError& e)
    {
        std::cerr << "error: " << e.What() << std::endl;
    }

}
