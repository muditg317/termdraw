#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <cassert>
#include <Eigen/Eigen>

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
 * @brief setup for 
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
 * @brief set the frame rate for animation
 * double the new frame rate
 */
void frameRate(double);

/**
 * @brief update the pixel buffer however desired before the next render call
 * 
 */
void update(void);

// void render(void);

void clean(void);

int graphics_main(int argc, char *argv[]);

#ifdef GRAPHICS_MAIN
// # ifndef GRAPHICS_HPP_MAIN
// # define GRAPHICS_HPP_MAIN
int main(int argc, char *argv[]) {
  graphics_main(argc, argv);
}
// # endif
#endif


#endif