#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>

#ifdef USE_EPOXY
#include <epoxy/gl.h>
#else
#include <SDL2/SDL_opengles2.h>
#endif

#include <iostream>
#include <string>
#include <cassert>
#include <chrono>
#include <memory>

/***
 * triangle object
 */

struct Triangle
{
    int width, height;
    GLuint vert_id, frag_id;
    GLuint prog_id, geom_id;
    GLint u_time_loc;
    GLfloat elapsed_time_;
   
    enum { Position_loc, Color_loc };

    Triangle()
      : width(400), height(300)
      , vert_id(0), frag_id(0)
      , prog_id(0), geom_id(0)
      , u_time_loc(-1)
      , elapsed_time_(0)
    {
      init();
    }

	void init()
	{
	   printf("init()\n");

	   glClearColor(.3f, .3f, .3f, 1.f);

	   auto load_shader = [](GLenum type, const char *src) -> GLuint
	   {
		  const GLuint id = glCreateShader(type);
		  assert(id);
		  glShaderSource(id, 1, &src, nullptr);
		  glCompileShader(id);
		  GLint compiled = 0;
		  glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
		  if (compiled == GL_FALSE) {
		    GLint logSize = 0;
		    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logSize);
		    GLchar * infoLog = new GLchar[logSize + 1];
		    glGetShaderInfoLog(id, logSize, nullptr, infoLog);
		    std::string shader_desc =  (type == GL_VERTEX_SHADER ? "vertex" : "fragment");
		    printf("Compilation of %s shader failed, here is log:\n %s \n", shader_desc.c_str(), infoLog);
		    assert(false);
		  }
		  //assert(compiled);
		  return id;
	   };

	   vert_id = load_shader(
		GL_VERTEX_SHADER,
		"attribute vec4 a_position;              \n"
		"attribute vec4 a_color;                 \n"
		"uniform float u_time;                   \n"
		"varying vec4 v_color;                   \n"
		"void main()                             \n"
		"{                                       \n"
		"    float sz = sin(u_time);             \n"
		"    float cz = cos(u_time);             \n"
		"    mat4 rot = mat4(                    \n"
		"     cz, -sz, 0,  0,                    \n"
		"     sz,  cz, 0,  0,                    \n"
		"     0,   0,  1,  0,                    \n"
		"     0,   0,  0,  1                     \n"
		"    );                                  \n"
		"    gl_Position = a_position * rot;     \n"
		"    v_color = a_color;                  \n"
		"}                                       \n"
	   );
	   printf("- vertex shader loaded\n");

	   frag_id = load_shader(
		GL_FRAGMENT_SHADER,
		"precision mediump float;                \n"
		"varying vec4 v_color;                   \n"
		"void main()                             \n"
		"{                                       \n"
		"    gl_FragColor = v_color;             \n"
		"}                                       \n"
	   );
	   printf("- fragment shader loaded\n");

	   prog_id = glCreateProgram();
	   assert(prog_id);
	   glAttachShader(prog_id, vert_id);
	   glAttachShader(prog_id, frag_id);
	   glBindAttribLocation(prog_id, Triangle::Position_loc, "a_position");
	   glBindAttribLocation(prog_id, Triangle::Color_loc, "a_color");
	   glLinkProgram(prog_id);
	   GLint linked = 0;
	   glGetProgramiv(prog_id, GL_LINK_STATUS, &linked);
	   assert(linked);
	   u_time_loc = glGetUniformLocation(prog_id, "u_time");
	   assert(u_time_loc >= 0);
	   glUseProgram(prog_id);
	   printf("- shader program linked & bound\n");

	   struct Vertex { float x, y, z; unsigned char r, g, b, a; };
	   const Vertex vtcs[] {
		{  0.f,  .5f, 0.f,   255, 0, 0, 255 },
		{ -.5f, -.5f, 0.f,   0, 255, 0, 255 },
		{  .5f, -.5f, 0.f,   0, 0, 255, 255 }
	   };
	   glGenBuffers(1, &geom_id);
	   assert(geom_id);
	   glBindBuffer(GL_ARRAY_BUFFER, geom_id);
	   glBufferData(GL_ARRAY_BUFFER, sizeof(vtcs), vtcs, GL_STATIC_DRAW);
	   auto offset = [](size_t value) -> const GLvoid * { return reinterpret_cast<const GLvoid *>(value); };
	   glVertexAttribPointer(Triangle::Position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offset(0));
	   glEnableVertexAttribArray(Triangle::Position_loc);
	   glVertexAttribPointer(Triangle::Color_loc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), offset(3 * sizeof(float)));
	   glEnableVertexAttribArray(Triangle::Color_loc);
	   printf("- geometry created & bound\n");
	}

	void resize(int w, int h)
	{
	   printf("resize(%d, %d)\n", w, h);
	   
	   width = w;
	   height = h;
	}

    void update(float f) {
      elapsed_time_ += f;
    }

	void draw()
	{
	   glViewport(0, 0, width, height);

	   glUniform1f(u_time_loc, elapsed_time_);
	   glDrawArrays(GL_TRIANGLES, 0, 3);
	   
	}
};

/***
 * main.cpp
 */

// Whether we should quit. It is most convenient for this to be filescope right now.
static bool done = false;

std::string align(std::string in, int num_tabs) {
	for (int i = (num_tabs * 8 - in.size()); i > 0; --i) {
		in += " ";
	}
	return in;
}

void log_gl_value(const std::string & name, SDL_GLattr sdl_enum) {
	int val;
	if (SDL_GL_GetAttribute(sdl_enum, &val)) { std::cerr << "(error?) "; }
	std::cout << name << val << std::endl;
}

void output_gl_diagnostics() {
	auto gl_version 	= reinterpret_cast< char const * >(glGetString(GL_VERSION));
	auto gl_renderer 	= reinterpret_cast< char const * >(glGetString(GL_RENDERER));
	auto gl_vendor	 	= reinterpret_cast< char const * >(glGetString(GL_VENDOR));
	auto glsl_version 	= reinterpret_cast< char const * >(glGetString(GL_SHADING_LANGUAGE_VERSION));
	auto gl_extensions	= reinterpret_cast< char const * >(glGetString(GL_EXTENSIONS));

	if (!(gl_version && gl_renderer && gl_vendor && glsl_version)) {
		std::cerr << "Could not obtain complete GL version info... GL context might not exist?!?\n";
	}

	std::cout << align("GL version:", 3) << (gl_version ? gl_version : "(null)") << std::endl;
	std::cout << align("GL renderer:", 3) << (gl_renderer ? gl_renderer : "(null)") << std::endl;
	std::cout << align("GL vendor:", 3) << (gl_vendor ? gl_vendor : "(null)") << std::endl;
	std::cout << align("GLSL version:", 3) << (glsl_version ? glsl_version : "(null)") << std::endl;
	std::cout << align("GL extensions:", 3) << (gl_extensions ? gl_extensions : "(null)") << std::endl;
	log_gl_value(align("Red Size:", 3), SDL_GL_RED_SIZE);
	log_gl_value(align("Blue Size:", 3), SDL_GL_BLUE_SIZE);
	log_gl_value(align("Green Size:", 3), SDL_GL_GREEN_SIZE);
	log_gl_value(align("Alpha Size:", 3), SDL_GL_ALPHA_SIZE);
	log_gl_value(align("Depth Size:", 3), SDL_GL_DEPTH_SIZE);
	log_gl_value(align("Double Buffer:", 3), SDL_GL_DOUBLEBUFFER);
}

struct SDL_graphics {
	int width_;
	int height_;
	SDL_Window * window_;
	SDL_GLContext context_;

	SDL_graphics(int width, int height)
		: width_(width)
		, height_(height)
		, window_(nullptr)
		, context_(nullptr)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2); 
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0); 
		// Probably not necessary but can't hurt

		window_ = SDL_CreateWindow("DEMO", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
				           SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE /*| SDL_WINDOW_ALLOW_HIGHDPI*/);
		if (window_ == nullptr) { std::cerr << "Error when creating SDL GL Window: '" << SDL_GetError() << "'\n"; }

		context_ = SDL_GL_CreateContext(window_);
		if (!context_) { std::cerr << "Error when creating SDL GL Context: '" << (SDL_GetError()) <<"'\n"; }


		int w, h;
		SDL_GetWindowSize(window_, &w, &h);		

		output_gl_diagnostics();
	}
};

static std::string locate_assets();

// Timing faciltiies

typedef unsigned int uint;

namespace util {
typedef std::chrono::high_resolution_clock clock;
typedef std::chrono::duration<uint, std::milli> ms;

inline uint time_diff_ms(const clock::time_point & b, const clock::time_point & a) {
  return std::chrono::duration_cast<ms>(b - a).count();
}

// Given a time_point reference, update it to now, and return uint representing number of ms passed
inline uint stopwatch_ms(clock::time_point & c) {
  clock::time_point now{clock::now()};
  uint result = time_diff_ms(now, c);
  c = now;
  return result;
}
}

// Main program object
struct program {
	SDL_graphics graphics_;
	std::unique_ptr<Triangle> triangle_;
    util::clock::time_point last_ticks_;

	program(int width, int height)
		: graphics_(width, height)
		, triangle_(new Triangle())
        , last_ticks_(util::clock::now())
	{
      triangle_->resize(width, height);
	}

    void loop_iteration() {
      // Update
      {
        uint ticks = util::stopwatch_ms(last_ticks_);
        triangle_->update(static_cast<float>(ticks) / 1000.0f);
      }

      // Draw
      {
		glClear(GL_COLOR_BUFFER_BIT);
		triangle_->draw();

		// Now flip the frame
		SDL_GL_SwapWindow(graphics_.window_); //->swap_buffers();
      }

    }
};

static Uint32 last_time = 0;

void loop_iteration(program* prog)
{
    prog->loop_iteration();
	// Now render graphics

	done |= SDL_QuitRequested();
}


int main(int argc, char* argv[])
{
//	SDL_Init(SDL_INIT_VIDEO);
		SDL_SetMainReady(); // Note: Emscripten crashes if SDL_INIT_TIMER is passed here
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
			std::cerr << "SDL_Init failed!\n";
			std::cerr << SDL_GetError() << std::endl;
			throw 42;
		}

	int width = 800, height = 600;

	// The mechanism of "emscripten_set_main_loop" is a bit opaque, better not to make this a stack object.
	program * me = new program(width, height);

	// Now enter main loop
	last_time = SDL_GetTicks();

#ifdef EMSCRIPTEN
	emscripten_set_main_loop_arg((em_arg_callback_func)loop_iteration, me, 0, 1);
#else
	while (!done) {
		loop_iteration(me);
	}
        delete me;
#endif


	SDL_Quit();

	return 0;
}
