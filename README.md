# cl
CL, Originally standing for ColorLayout, is a small console app, to extract feature vectors from images.

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
>                                                     use. cl, cedd, fcth or jcd
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
>Arguments:
>  images/videos                                      Images or video to
>                                                     process.

Example:
>\>\>\> cl -d fcth ae.jpg
>
> ae.jpg: DsCAAIaAgIKAQICAooCAoICAgICAgICApICAwISAgIKAoICAgICAgICAgICAgICAhoCAoICAgICAoA==
