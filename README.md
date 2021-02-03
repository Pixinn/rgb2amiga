**amiga2rgb** is a small program to convert a modern RGB image (jpeg, png, etc.) to an IFF/ILMB tailored for the Amiga 500. 

## How to use

>  Rgb2Amiga  [-s <string>] [-d <string>] [-c <string>] -o <string> -i string> [--] [--version] [-h]

Where:

* -s <string>,  --size <string> \
	Targeted size in WidthxHeight format. Defaults to "320x256" \
	Optionnal suffix: '!' ignore the original aspect ratio.
* -d <string>,  --dither <string> \
	Use dithering. Set to <true> or <false>. Defaults to false.
* -c <string>,  --colors <string> \
	Number of colors to use. Defaults to "32".
* -o <string>,  --output <string> \
	(required)  Output file.
* -i <string>,  --input <string> \
	(required)  Input file to process.
* --,  --ignore_rest \
	Ignores the rest of the labeled arguments following this flag.
* --version \
	Displays version information and exits.
* -h,  --help \
	Displays usage information and exits.

## How to build

*rgb2amiga* comes with VisualStudio project and solution so it can be built as-is on a Windows System if the dependency requirement is fullfilled.

Nothing should prevent it to be built on any other architecture, so writing a CMakeList would be nice 😉

### Dependencies

*rgb2amiga* depends on *ImageMagick* 6.9 and its *Magick++* API, compiled for **x64** target. Install ImageMagick on your machine and sets the **MAGICK_HOME** environment variable to its location.

### Build

Open  the *Rgb2Amiga.sln* and build.
