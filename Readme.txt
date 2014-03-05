To compile on windows:

Install visual studio (only checked with 2012, but should work with 2013)

Download CMAKe,
choose source folder "source"
choose build folder "bin"

Configure CMake, note: Visual studio 2012 is "Visual studio 11"


open solution file and select Release or Debug
build solution



To Compile on osx:
Download SDL for OSX from http://www.libsdl.org/
Download SDL_TTF for OSX from http://www.libsdl.org/projects/SDL_ttf/
open dmg files, and install as specified in "readme.txt" (for both)

Download CMake,
choose source folder "source"
choose build folder "bin"

configure cmake, use xcode
build solution


For both operating systems:
copy the contents of source/game (not the folder itself) to the Debug or Release directory located in the bin directory (after build)
run the binary (not through the build system)

(For windows you also need all the dlls located in libs\windows\_alldlls)
