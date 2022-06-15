#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <cassert>
#include <chrono>
#include <cstdarg>

#include <Eigen/Eigen>
#include <fmt/format.h>


#define PIX_BUF_MATRIX_T bool
template<int T1, int T2>
using PixBuf = Eigen::Matrix<PIX_BUF_MATRIX_T, T1, T2>;

typedef PixBuf<Eigen::Dynamic, Eigen::Dynamic> PixelBuffer;

class GraphicsApplication;

extern GraphicsApplication *app;

double getFrameRate();
#define FRAME_RATE (app->getFrameRate())
#define DEFAULT_FRAME_RATE 15.0
#define MAX_FR_RECOMMENDATION 60.0

// millis change in time per frame
#define DELTA_T_MS (1000.0 / (FRAME_RATE))
#define DELTA_T_SEC (1.0 / (FRAME_RATE))

// int getWidth();
#define WIDTH (app->getWidth())

// int getHeight();
#define HEIGHT (app->getHeight())

// PixelBuffer& getPixelBuffer();
#define BUFFER (app->getPixelBuffer())

#define WIDTH_SCALE 2
#define CONSOLE_WIDTH (WIDTH/WIDTH_SCALE)
#define HEIGHT_SCALE 4
#define CONSOLE_HEIGHT (HEIGHT/HEIGHT_SCALE)

#define BYTES_PER_CONSOLE_CHAR 3
#define CONSOLE_LINE_SIZE ((CONSOLE_WIDTH)*(BYTES_PER_CONSOLE_CHAR) + 1) /* +1 added to account for new lines*/

static_assert(HEIGHT_SCALE == WIDTH_SCALE * 2, "Height must be scaled exactly twice as much as width!!");

class GraphicsApplication {
 public:
  // static bool appCreated; // MUST BE FALSE
  GraphicsApplication();

  /**
   * @brief setup for graphics - called directly from main
   */
  void setup(int argc, char *argv[]);

  /**
   * @brief update the pixel buffer however desired before the next render call
   */
  void update(void);

  void render(void);

  double getFrameRate(void);
  int getWidth(void);
  int getHeight(void);
  PixelBuffer& getPixelBuffer(void);
  
//  protected:

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

 private:
  void allocate_buffers(void);


  double _frameRate;
  int _width;
  int _height;
  PixelBuffer pixelBuffer;
  char *consoleBuffer;

  bool pixelBufferAllocated;

  bool dimsSet;
};

void _graphics_print_string(std::string);

inline void graphics_printf(const char *format, ...) {
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

extern bool quit_application;

#define MAX_FUNCTION_REGISTRATIONS 5

typedef void preloopFunc(int argc, char *argv[]);
void registerPreloop(preloopFunc);

typedef void loopFunc(void);
void registerLoop(loopFunc);

typedef void finishFunc(int signum);
void registerFinish(finishFunc);

preloopFunc graphics_preloop;
loopFunc graphics_loop;
finishFunc graphics_finish;

int graphics_main(int argc, char *argv[]);

#ifdef GRAPHICS_MAIN
int main(int argc, char *argv[]) {
  graphics_main(argc, argv);
}
#endif


#endif