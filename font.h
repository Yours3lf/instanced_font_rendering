#ifndef font_h
#define font_h

#include "mymath/mymath.h"

#include "GL/glew.h"

#include <map>
#include <list>
#include <string>
#include <vector>

/*
 * Based on Shikoba
 */

struct glyph;
class font;
class font_inst;

#define FONT_LIB_VBO_SIZE 6

struct fontscalebias
{
  mm::vec4 vertscalebias;
  mm::vec4 texscalebias;

  fontscalebias( mm::vec2 vertscale, mm::vec2 vertbias, mm::vec2 texscale, mm::vec2 texbias ) :
    vertscalebias( mm::vec4( vertscale, vertbias ) ), texscalebias( mm::vec4( texscale, texbias ) ) {}
};

class library
{
    friend class font;
    friend class face;
    friend class font_inst;
  private:
    void* the_library;
    mm::uvec2 texture_pen;
    GLint texture_row_h;
    GLuint tex; //font texture
    mm::uvec2 texsize;
    GLuint vao; //vao
    GLuint vbos[FONT_LIB_VBO_SIZE]; //vbos
    std::vector<fontscalebias> font_data;
    GLuint the_shader; //shader program
    bool is_set_up;
    std::vector<font_inst*> instances;

    void delete_glyphs();

    void* get_library()
    {
      return the_library;
    }

    GLuint& get_shader()
    {
      return the_shader;  //load shader externally
    }

    mm::uvec2 get_texsize()
    {
      return texsize;
    }

    mm::uvec2& get_texture_pen()
    {
      return texture_pen;
    }

    GLint& get_tex_row_h()
    {
      return texture_row_h;
    }

    GLuint get_tex()
    {
      return tex;
    }

    size_t get_font_data_size()
    {
      return font_data.size();
    }

    fontscalebias& get_font_data( size_t i )
    {
      return font_data[i];
    }

    void set_up();
    void destroy();

    void bind_shader()
    {
      glUseProgram( the_shader );
    }

    void bind_texture()
    {
      glBindTexture( GL_TEXTURE_RECTANGLE, tex );
    }

    void bind_vao()
    {
      glBindVertexArray( vao );
    }

    template< class t >
    void update_scalebiascolor( unsigned int i, const t& tt )
    {
      glBindBuffer( GL_ARRAY_BUFFER, vbos[i] );

      if( tt.size() > 0 )
        glBufferData( GL_ARRAY_BUFFER, sizeof( mm::vec4 ) * tt.size(), &tt[0], GL_DYNAMIC_DRAW );
    }

    bool expand_tex();

    void add_font_data( const fontscalebias& fd )
    {
      font_data.push_back( fd );
    }
  protected:
    library(); //singleton
    library( const library& );
    library( library && );
    library& operator=( const library& );
    ~library();
  public:

    static library& get()
    {
      static library instance;
      return instance;
    }
};

#ifdef _WIN32
typedef unsigned int uint32_t;
#endif

//this corresponds to a font file '*.ttf'
//meaning if you'd like to switch to another font-type
//you have to switch font instances
//you can switch between sizes though
class font_inst
{
  private:
  protected:
  public:
    class face
    {
        friend class font;
        friend class library;
      private:
        unsigned int size;
        void* the_face; //FT_Face
        std::map< unsigned int, std::map<uint32_t, glyph> >* glyphs;

        void set_size( unsigned int val );
        bool load_glyph( uint32_t val );

        unsigned int get_size()
        {
          return size;
        }

        glyph& get_glyph( uint32_t i );
        bool has_glyph( uint32_t i );
        float advance( const uint32_t prev, const uint32_t next = 0 );
        float height();
        float ascender();
        float descender();
      protected:
      public:
        face();
        face( const std::string& filename, unsigned int index = 0 );
        ~face();
    }* the_face;

    font_inst() : the_face( 0 ) {}
    ~font_inst()
    {
      delete the_face;
    }
};

class font
{
  private:
    mm::uvec2 screensize;
    mm::frame<float> font_frame;

    void add_glyph( font_inst& f, uint32_t c, int counter = 0 );
  protected:
    font() {} //singleton
    font( const font& );
    font( font && );
    font& operator=( const font& );
  public:
    void load_font( const std::string& filename, font_inst& font_ptr, unsigned int size );
    mm::uvec2 add_to_render_list( const std::wstring& text, font_inst& font_ptr, mm::vec4 color = mm::vec4( 1 ), mm::uvec2 pos = mm::uvec2() );
    void render();

    void set_size( font_inst& f, unsigned int s );

    void resize( mm::uvec2 ss );

    void destroy()
    {
      library::get().destroy();
    }

    GLuint& get_shader()
    {
      return library::get().get_shader();
    }

    static font& get()
    {
      static font instance;
      return instance;
    }
};

#endif
