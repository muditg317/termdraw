#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <cassert>
#include <chrono>
#include <cstdarg>

#include <Eigen/Eigen>
#include <fmt/format.h>

enum TermPix {
  OFF = 0,
  ON = 1,
  TERM_PIX_N
};

#define PIX_BUF_MATRIX_T bool
template<int T1, int T2>
using PixBuf = Eigen::Matrix<PIX_BUF_MATRIX_T, T1, T2>;

typedef PixBuf<Eigen::Dynamic, Eigen::Dynamic> PixelBuffer;

double getFrameRate();
#define FRAME_RATE (getFrameRate())
#define DEFAULT_FRAME_RATE 15.0
#define MAX_FR_RECOMMENDATION 60.0

// millis change in time per frame
#define DELTA_T_MS (1000.0 / (FRAME_RATE))
#define DELTA_T_SEC (1.0 / (FRAME_RATE))

int getWidth();
#define WIDTH (getWidth())

int getHeight();
#define HEIGHT (getHeight())

PixelBuffer& getPixelBuffer();
#define BUFFER (getPixelBuffer())

#define WIDTH_SCALE 2
#define CONSOLE_WIDTH (WIDTH/WIDTH_SCALE)
#define HEIGHT_SCALE 4
#define CONSOLE_HEIGHT (HEIGHT/HEIGHT_SCALE)

#define BYTES_PER_CONSOLE_CHAR 3
#define CONSOLE_LINE_SIZE ((CONSOLE_WIDTH)*(BYTES_PER_CONSOLE_CHAR) + 1) /* +1 added to account for new lines*/

static_assert(HEIGHT_SCALE == WIDTH_SCALE * 2, "Height must be scaled exactly twice as much as width!!");

/**
 * @brief setup for graphics
 * 
 */
void setup(int argc, char *argv[]);

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


/**
 * @brief update the pixel buffer however desired before the next render call
 */
void update(void);

/**
 * @brief Zero out the pixel buffer
 */
void clean(void);

/**
 * @brief application code for the end of the execution
 * 
 */
void finish(void);

void graphics_preloop(int argc, char *argv[]);
void graphics_loop();
void graphics_finish(int signum  = 0);

int graphics_main(int argc, char *argv[]);

#ifdef GRAPHICS_MAIN
int main(int argc, char *argv[]) {
  graphics_main(argc, argv);
}
#endif


#endif