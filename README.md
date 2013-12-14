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

Example: 
```c++ 
//load in the shaders with your method, get_shader() gives you a ref to the shader program 
load_shader( font::get().get_shader(), GL_VERTEX_SHADER, "../shaders/font/font.vs" ); 
load_shader( font::get().get_shader(), GL_FRAGMENT_SHADER, "../shaders/font/font.ps" ); 

uvec2 screen = uvec2( 1280, 720 );

font_inst instance; //this holds your font type and the corresponding sizes
font::get().resize( screen ); //set screen size
font::get().load_font( "../resources/font.ttf", //where your font is
                       instance, //font will load your font into this instance
                       22 ); //the font size

vec3 color = vec3( 0.5, 0.8, 0.5 ); //rgb [0...1]
uvec2 pos = uvec2( 10, 20 ); //in pixels

std::wstring text = L"hello world\n"; //what to display

rendering:
while(true) //your ordinary rendering loop
{
  clear_screen();
  //...
  //optionally bind fbo here to render to texture
  //...
  font::get().add_to_text( instance, text + L"_" ); //feed the font
  font::get().render( instance, color, pos );
  //...
  swap_buffers();
}
```
