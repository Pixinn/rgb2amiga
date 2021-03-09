**amiga2rgb** is a small program to convert modern RGB images (jpeg, png, etc.) to an IFF/ILMB tailored for the Amiga 500. 

## How to use

> Rgb2Amiga  [-p] [-d] [-s <string>] [-c <string>] -o <string> -i <string> [--] [--version] [-h]

Where:

	*   -f <format slection>,  --format <format slection>
		Save as iff-ilbm (default) or png-gpl (PNG + Gimp palette).

	*   p <scale>,  --preview <scale>
     	        Open a window to display a scaled preview. Defaults to no preview.

	*  -d,  --dither	
		Use dithering.

	*   -s <string>,  --size <string>
		Targeted size in WidthxHeight format. Defaults to "320x256"
		Optionnal suffix: '!' ignore the original aspect ratio.
	
	*   -c <string>,  --colors <string>		
		Number of colors to use. Defaults to "32".
		
	*   -o <string>,  --output <string> (accepted multiple times)
		(required)  Output file.

	*   -i <string>,  --input <string> (accepted multiple times)
		(required)  Input file to process.

	*   --,  --ignore_rest
		Ignores the rest of the labeled arguments following this flag.

	*   --version
		Displays version information and exits.

	*   -h,  --help
		Displays usage information and exits.
		
You can provide multiple input images. If so, the output images will all share the same palette.

## How to build
### Dependencies

* **Magick++** from ImageMagick 6 and **SDL2**
    * On Windows
      	* ImageMagick
		* Download ImageMagick 6 from the [official website](https://legacy.imagemagick.org/script/install-source.php).
		* Compile it in *Dynamic Multithreaded*.
		* Provide an environment variable called *MAGICK_HOME* and pointing to the root folder of *ImageMagick*.
		* Copy *ImageMagick-config* from the *script/* folder to *MAGICK_HOME*.
	 * SDL2
		* Download the latest version of the *development library* from the [official website](https://www.libsdl.org/download-2.0.php) and unarchive it.
		* Provide an environment variable called *SDL2_HOME* and pointing to the root of the SDL2 library.

    * On Linux, install libmagick++-dev for version 6 and libsdl2-dev
	  > sudo apt install libmagick++-6.q16-dev libsdl2-dev

### Build the binary

A *CMakeList* is provided:
  > mkdir build
  
  >cd build

  > cmake .. -DCMAKE_BUILD_TYPE=Release

  > cmake --build . --config release
  
