# cl

CL, Originally standing for ColorLayout, is a small console app, to extract features from images.
Has the abiliy to output files that can be read by [ImSearch](https://gitlab.com/Hupie69xd/imsearch)

# Usage

> Usage: cl [options] images/videos
> Extract image descriptors.
> 
> Options:
>  -?, -h, --help                                     Displays help on
>                                                     commandline options.
>                                                     
>  --help-all                                         Displays help including Qt
>                                                     specific options.
>                                                     
>  -v, --version                                      Displays version
>                                                     information.
>                                                     
>  -d, --hasher <hasher>                              Which image descriptor to
>                                                     use. cl, cedd, fcth, jcd,
>                                                     phash or dhash
>  
>  -n, --name <name>                                  Name field of the Isc
>                                                     File, default is empty
>                                                     string.
>  
>  --no-title                                         Don't use input files as
>                                                     titles in the Isc Index.
>  
>  --isc                                              Produce an Isc file to be
>                                                     loaded by ImSearch.
>
>  --license                                         Display short license information.
>
>  -x                                                Don't display the copyright notice.
>
>Arguments:
>
>  images/videos                                      Images or video to
>                                                     process.

Example:
>\>\>\> cl -d fcth ae.jpg
>
> ae.jpg: DsCAAIaAgIKAQICAooCAoICAgICAgICApICAwISAgIKAoICAgICAgICAgICAgICAhoCAoICAgICAoA==

# Compiling

Depending on your QT version you might need to remove the files videoextractor.h/.cpp,
as these use features only availeble in QT version 6 and higher.

### Windows

1. Download the Windows [FFmpeg binaries](https://ffmpeg.org/download.html) to link against
2. Make sure the FFmpeg library and headers can be found by the compiler,
either by adding it to the pro file or by adding the location to LIBPATH and INCLUDEPATH
3. Open the project with QT Creator
4. Compile

### Debian (and other linux dist's)

1. Install the FFmpeg libraries with `sudo apt install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev`
2. Open the project with QT Creator
3. Compile
