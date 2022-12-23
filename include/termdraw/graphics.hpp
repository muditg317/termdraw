#pragma once

#include <termdraw/application.hpp>
#include <termdraw/base.hpp>
#include <termdraw/capability.hpp>

#include <cassert>
#include <chrono>
#include <cstdarg>

#include <Eigen/Eigen>
#include <fmt/format.h>


#define PIX_BUF_MATRIX_T bool
template<int T1, int T2>
using PixBuf = Eigen::Matrix<PIX_BUF_MATRIX_T, T1, T2>;

typedef PixBuf<Eigen::Dynamic, Eigen::Dynamic> PixelBuffer;

typedef PIX_BUF_MATRIX_T pixelValue;
constexpr pixelValue WHITE = 1;
constexpr pixelValue BLACK = 0;

#define GRAPHICS_CAPABILITY_NAME "graphics"


#define APP_GRAPHICS (termdraw::app->getCapability<termdraw::graphics::Graphics>())
#define GRAPHICS (getCapability<termdraw::graphics::Graphics>())

// double getFrameRate();
#define FRAME_RATE (APP_GRAPHICS.getFrameRate())
#define DEFAULT_FRAME_RATE 15.0
#define MAX_FR_RECOMMENDATION 60.0

// millis change in time per frame
#define DELTA_T_MS (1000.0 / (FRAME_RATE))
#define DELTA_T_SEC (1.0 / (FRAME_RATE))

// int getWidth();
#define WIDTH (APP_GRAPHICS.getWidth())

// int getHeight();
#define HEIGHT (APP_GRAPHICS.getHeight())

// PixelBuffer& getPixelBuffer();
#define BUFFER (APP_GRAPHICS.getPixelBuffer())

#define WIDTH_SCALE 2
#define CONSOLE_WIDTH (WIDTH/WIDTH_SCALE)
#define HEIGHT_SCALE 4
#define CONSOLE_HEIGHT (HEIGHT/HEIGHT_SCALE)

#define BYTES_PER_CONSOLE_CHAR 3
#define CONSOLE_LINE_SIZE ((CONSOLE_WIDTH)*(BYTES_PER_CONSOLE_CHAR) + 1) /* +1 added to account for new lines*/

static_assert(HEIGHT_SCALE == WIDTH_SCALE * 2, "Height must be scaled exactly twice as much as width!!");

namespace termdraw {

namespace graphics {

class Graphics : public Capability {

  typedef std::function<void(int, char **)> setupFunc_t;
  typedef std::function<void(void)> updateFunc_t;

 public:
  Graphics(setupFunc_t, updateFunc_t);
  // virtual ~Graphics();

  double getFrameRate(void) const;
  int getWidth(void) const;
  int getHeight(void) const;
  PixelBuffer& getPixelBuffer(void);
  
  /**
   * @brief set the dimensions for the graphics buffer
   * int width
   * int height
   */
  void display_size(int, int);

  /**
   * @brief set the dimensions of the graphics buffer based on the console size
   * int number of lines to reserve for non-graphics
   */
  void display_size_based_on_console(int = 0);
  
  /**
   * @brief set the frame rate for animation
   * double the new frame rate
   */
  void frameRate(double);

  /**
   * @brief Zero out the pixel buffer
   */
  void clean(void);

  void fill(pixelValue); // fill with a single value

 private:
 
  /**
   * @brief setup for graphics - called directly from main
   */
  // virtual void setup(int argc, char *argv[]) = 0;
  setupFunc_t setup;

  /**
   * @brief update the pixel buffer however desired before the next render call
   */
  // virtual void update(void) = 0;
  updateFunc_t update;

  void allocate_buffers(void);
  void render(void);

  preloopFunc graphics_preloop;
  loopFunc graphics_loop;
  finishFunc graphics_finish;

  double _frameRate;
  int _width;
  int _height;
  PixelBuffer pixelBuffer;
  char *consoleBuffer;

  bool pixelBufferAllocated;

  bool dimsSet;

  template<class... Capabilities>
  friend class Application;
};


void _graphics_print_string(std::string result);

inline void printf(const char *format, ...) {
  // ===== Option if buffer sizing becomes an issue
  // int size = snprintf(NULL, 0, "%d", 132);
  // char * a = malloc(size + 1);
  // =================
  static char buffer[1000];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  // std::printf("Got printf call with format |%s| and result |%s|\n", format, buffer);
  _graphics_print_string(*new std::string(buffer));
  va_end(args);
}

}  // namespace graphics

}  // namespace termdraw