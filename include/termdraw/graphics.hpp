#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <cassert>

enum TermPix {
  OFF = 0,
  ON = 1,
  TERM_PIX_N
};

double getFrameRate();
#define FRAME_RATE (getFrameRate())
#define DEFAULT_FRAME_RATE 15.0
#define MAX_FR_RECOMMENDATION 60.0

// millis change in time per frame
#define DELTA_T (1000.0 / (FRAME_RATE))

int getWidth();
#define WIDTH (getWidth())
#define WIDTH_SCALE (1)
#define CONSOLE_WIDTH (WIDTH/WIDTH_SCALE)

int getHeight();
#define HEIGHT (getHeight())
#define HEIGHT_SCALE (2)
#define CONSOLE_HEIGHT (HEIGHT/HEIGHT_SCALE)

static_assert(HEIGHT_SCALE == WIDTH_SCALE * 2, "Height must be scaled exactly twice as much as width!!");

TermPix **const getPixelBuffer();
#define BUFFER (getPixelBuffer())

/**
 * @brief setup for 
 * 
 */
void setup(void);

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
 * @brief update the pixel buffer however desired before the next draw loop
 * 
 */
void update(void);

void draw(void);

int graphics_main(void);

#ifndef GRAPHICS_IMPL_ONLY
// # ifndef GRAPHICS_HPP_MAIN
// # define GRAPHICS_HPP_MAIN
int main(void);
// # endif
#endif


#endif