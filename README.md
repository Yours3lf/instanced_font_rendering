instanced_font_rendering
========================

This is a tiny font rendering library originally 
based on Shikoba. ( https://github.com/Queatz/Shikoba )

Depends on libmymath. ( https://github.com/Yours3lf/libmymath )
Depends on Freetype. ( http://www.freetype.org/ )

The demo needs SFML ( http://sfml-dev.org/ ) and 
GLEW ( http://glew.sourceforge.net/ ) to run.

For usage example see main.cpp

Building:
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. 
make

Running:
./instanced_font_rendering

Please note that you need to provide a font file here: 
resources/font.ttf

Performance of the demo on my PC (A8-4500m apu): 1.06-1.09 ms

In visual studio set build type to Release to enjoy full speed.
In visual studio set the instance font rendering project to the
default startup project to be able to debug it.
