#include <iostream>
#include <map>
#include <algorithm>
#include <sstream>
#include <fstream>

#include "SFML/Window.hpp"

#include "mymath/mymath.h"

#include "font.h"

#define STRINGIFY(s) #s
#define INFOLOG_SIZE 4096

using namespace std;
using namespace mymath;

void compile_shader( const char* text, const GLuint& program, const GLenum& type );
void link_shader( const GLuint& shader_program );
void load_shader( GLuint& program, const GLenum& type, const string& filename );

int main( int argc, char** argv )
{
  map<string, string> args;

  for( int c = 1; c < argc; ++c )
  {
    args[argv[c]] = c + 1 < argc ? argv[c + 1] : "";
    ++c;
  }

  cout << "Arguments: " << endl;
  for_each( args.begin(), args.end(), []( pair<string, string> p )
  {
    cout << p.first << " " << p.second << endl;
  } );

  uvec2 screen( 0 );
  bool fullscreen = false;
  bool silent = false;
  string title = "Basic CPP to get started";

  /*
   * Process program arguments
   */

  stringstream ss;
  ss.str( args["--screenx"] );
  ss >> screen.x;
  ss.clear();
  ss.str( args["--screeny"] );
  ss >> screen.y;
  ss.clear();

  if( screen.x == 0 )
  {
    screen.x = 1280;
  }

  if( screen.y == 0 )
  {
    screen.y = 720;
  }

  try
  {
    args.at( "--fullscreen" );
    fullscreen = true;
  }
  catch( ... ) {}

  try
  {
    args.at( "--help" );
    cout << title << ", written by Marton Tamas." << endl <<
         "Usage: --silent      //don't display FPS info in the terminal" << endl <<
         "       --screenx num //set screen width (default:1280)" << endl <<
         "       --screeny num //set screen height (default:720)" << endl <<
         "       --fullscreen  //set fullscreen, windowed by default" << endl <<
         "       --help        //display this information" << endl;
    return 0;
  }
  catch( ... ) {}

  try
  {
    args.at( "--silent" );
    silent = true;
  }
  catch( ... ) {}

  /*
   * Initialize the OpenGL context
   */

   //screen = uvec2( 1920, 1080 );
   //fullscreen = true;

  sf::Window the_window;
  the_window.create( sf::VideoMode( screen.x > 0 ? screen.x : 1280, screen.y > 0 ? screen.y : 720, 32 ), title, fullscreen ? sf::Style::Fullscreen : sf::Style::Default );

  if( !the_window.isOpen() )
  {
    cerr << "Couldn't initialize SFML." << endl;
    the_window.close();
    exit( 1 );
  }

  the_window.setVerticalSyncEnabled( true );

  GLenum glew_error = glewInit();

  glGetError(); //ignore glew errors

  cout << "Vendor: " << glGetString( GL_VENDOR ) << endl;
  cout << "Renderer: " << glGetString( GL_RENDERER ) << endl;
  cout << "OpenGL version: " << glGetString( GL_VERSION ) << endl;
  cout << "GLSL version: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << endl;

  if( glew_error != GLEW_OK )
  {
    cerr << "Error initializing GLEW: " << glewGetErrorString( glew_error ) << endl;
    the_window.close();
    exit( 1 );
  }

  if( !GLEW_VERSION_4_3 )
  {
    cerr << "Error: " << STRINGIFY( GLEW_VERSION_4_3 ) << " is required" << endl;
    the_window.close();
    exit( 1 );
  }

  //set opengl settings
  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LEQUAL );
  glFrontFace( GL_CCW );
  glEnable( GL_CULL_FACE );
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  glClearDepth( 1.0f );

  /*
   * Set up mymath
   */

  glViewport( 0, 0, screen.x, screen.y );

  /*
   * Set up the shaders
   */

  load_shader( font::get().get_shader(), GL_VERTEX_SHADER, "../shaders/font/font.vs" );
  load_shader( font::get().get_shader(), GL_FRAGMENT_SHADER, "../shaders/font/font.ps" );

  font_inst instance;
  font::get().resize( screen );
  int size = 22;
  font::get().load_font( "../resources/font.ttf", instance, size );

  font_inst instance2;
  font::get().load_font( "../resources/font2.ttf", instance2, size );

  std::wstring text;

  //text = L"hello world\n";
  //for( int c = 0; c < 43; ++c )
    text += L" 0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+!%/=()~|$[]<>#&@{},.-?:_;*`^'\".aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n";

  /*
   * Handle events
   */

  bool run = true;

  auto event_handler = [&]( const sf::Event & ev )
  {
    switch( ev.type )
    {
      case sf::Event::Closed:
        {
          run = false;
          break;
        }
      case sf::Event::TextEntered:
        {
          if( ev.text.unicode >= 32 && ev.text.unicode <= 127 )
            text += ev.text.unicode;

          break;
        }
      case sf::Event::KeyPressed:
        {
          if( ev.key.code == sf::Keyboard::Delete )
            if( !text.empty() )
              text.erase( --text.end() );

          if( ev.key.code == sf::Keyboard::BackSpace )
            if( !text.empty() )
              text.erase( --text.end() );

          if( ev.key.code == sf::Keyboard::Return )
            text += L"\n";

          if( ev.key.code == sf::Keyboard::Escape )
            run = false;

          if( ev.key.code == sf::Keyboard::Add )
          {
            ++size;
            font::get().set_size( instance, size );
          }

          if( ev.key.code == sf::Keyboard::Subtract )
          {
            if( size > 1 )
            {
              --size;
              font::get().set_size( instance, size );
            }
          }
        }
      default:
        break;
    }
  };

  /*
   * Render
   */

  sf::Clock timer;
  timer.restart();
  unsigned int frame_count = 0;

  sf::Clock thetimer;
  thetimer.restart();

  sf::Event the_event;

  while( run )
  {
    while( the_window.pollEvent( the_event ) )
    {
      event_handler( the_event );
    }

    glClearColor( 1, 1, 1, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    vec2 lastpos;
    //lastpos = font::get().add_to_render_list( L"", instance2, vec4( 1, 0, 0, 1 ) );
    //lastpos = font::get().add_to_render_list( L"hello ", instance2, vec4( 1, 0, 0, 1 ), lastpos );
    //lastpos = font::get().add_to_render_list( L"world\n", instance2, vec4( 0, 1, 0, 1 ), lastpos );
    mat4 mat = create_rotation( radians( -thetimer.getElapsedTime().asMilliseconds() * 0.001f ), vec3( 0, 0, 1 ) );
    //mat = mat * create_translation( vec3( 0, 10, 0 ) );
    lastpos = font::get().add_to_render_list( text + L"_" + L"\n", instance, vec4( vec3(0), 1 ), vec2( 0, lastpos.y ), mat );
    /**
    lastpos = font::get().add_to_render_list( L"\uE000\uE002\uE004\uE006Lorem ipsum dolor sit amet, consectetur adipiscing \uE007\uE005\uE003\uE001\n", instance, vec4( vec3(0),1 ), lastpos, vec4( 0.5, 0.8, 0.5, 1 ) );
    lastpos = font::get().add_to_render_list( L"elit. Vestibulum ultrices nibh vitae augue rhoncus, in \n", instance, vec4( vec3(0),1 ), lastpos );
    lastpos = font::get().add_to_render_list( L"porta dolor tristique. Donec quam risus, mollis eget \n", instance, vec4( vec3(0),1 ), lastpos );
    lastpos = font::get().add_to_render_list( L"mi vel, facilisis consectetur ipsum. Mauris a lacus \n", instance, vec4( vec3(0),1 ), lastpos );
    lastpos = font::get().add_to_render_list( L"vel sem commodo aliquet. Sed placerat ultricies \n", instance, vec4( vec3(0),1 ), lastpos );
    lastpos = font::get().add_to_render_list( L"augue, vel porttitor dui venenatis in. Vestibulum \n", instance, vec4( vec3(0),1 ), lastpos );
    lastpos = font::get().add_to_render_list( L"tortor augue, hendrerit sit amet mauris in, euismod \n", instance, vec4( vec3(0),1 ), lastpos );
    lastpos = font::get().add_to_render_list( L"laoreet elit. Nam adipiscing fringilla lobortis. Ut \n", instance, vec4( vec3(0),1 ), lastpos );
    lastpos = font::get().add_to_render_list( L"lacinia accumsan sapien sit amet tempus. Quisque et \n", instance, vec4( vec3(0),1 ), lastpos );
    lastpos = font::get().add_to_render_list( L"lorem nulla. Sed aliquam pellentesque porttitor. \n", instance, vec4( vec3(0),1 ), lastpos );
    lastpos = font::get().add_to_render_list( L"Fusce at dignissim purus. Nunc a augue pulvinar, \n", instance, vec4( vec3(0),1 ), lastpos );
    lastpos = font::get().add_to_render_list( L"tincidunt turpis quis, imperdiet turpis. Nullam eget \n", instance, vec4( vec3(0),1 ), lastpos );
    lastpos = font::get().add_to_render_list( L"est tempus, pulvinar lectus ut, lacinia est. Proin \n", instance, vec4( vec3(0),1 ), lastpos );
    lastpos = font::get().add_to_render_list( L"placerat tristique diam, a porttitor magna lobortis \n", instance, vec4( vec3(0),1 ), lastpos );
    /**/

    font::get().render();

    ++frame_count;

    if( timer.getElapsedTime().asMilliseconds() > 1000 && !silent )
    {
      int timepassed = timer.getElapsedTime().asMilliseconds();
      int fps = 1000.0f / ( ( float ) timepassed / ( float ) frame_count );

      string ttl = title;

      ss << " - FPS: " << fps
         << " - Time: " << ( float ) timepassed / ( float ) frame_count;

      ttl += ss.str();
      ss.str( "" );

      the_window.setTitle( ttl );

      frame_count = 0;
      timer.restart();
    }

    the_window.display();
  };

  font::get().destroy();

  return 0;
}

void compile_shader( const char* text, const GLuint& program, const GLenum& type )
{
  GLchar infolog[INFOLOG_SIZE];

  GLuint id = glCreateShader( type );
  glShaderSource( id, 1, &text, 0 );
  glCompileShader( id );

  GLint success;
  glGetShaderiv( id, GL_COMPILE_STATUS, &success );

  if( !success )
  {
    glGetShaderInfoLog( id, INFOLOG_SIZE, 0, infolog );
    cerr << infolog << endl;
  }
  else
  {
    glAttachShader( program, id );
    glDeleteShader( id );
  }
}

void link_shader( const GLuint& shader_program )
{
  glLinkProgram( shader_program );

  GLint success;
  glGetProgramiv( shader_program, GL_LINK_STATUS, &success );

  if( !success )
  {
    GLchar infolog[INFOLOG_SIZE];
    glGetProgramInfoLog( shader_program, INFOLOG_SIZE, 0, infolog );
    cout << infolog << endl;
  }

  glValidateProgram( shader_program );

  glGetProgramiv( shader_program, GL_VALIDATE_STATUS, &success );

  if( !success )
  {
    GLchar infolog[INFOLOG_SIZE];
    glGetProgramInfoLog( shader_program, INFOLOG_SIZE, 0, infolog );
    cout << infolog << endl;
  }
}

void load_shader( GLuint& program, const GLenum& type, const string& filename )
{
  ifstream f( filename );

  if( !f ) cerr << "Couldn't load shader: " << filename << endl;

  string str( ( istreambuf_iterator<char>( f ) ),
              istreambuf_iterator<char>() );

  if( !program ) program = glCreateProgram();

  compile_shader( str.c_str(), program, type );
  link_shader( program );
}
