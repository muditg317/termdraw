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

int getHeight();
#define HEIGHT (getHeight())

TermPix **const getPixelBuffer();
#define BUFFER (getPixelBuffer())

#define WIDTH_SCALE 2
#define CONSOLE_WIDTH (WIDTH/WIDTH_SCALE)
#define HEIGHT_SCALE 4
#define CONSOLE_HEIGHT (HEIGHT/HEIGHT_SCALE)
#define BYTES_PER_CONSOLE_CHAR 3

static_assert(HEIGHT_SCALE == WIDTH_SCALE * 2, "Height must be scaled exactly twice as much as width!!");


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

void clean(void);

int graphics_main(void);

#ifdef GRAPHICS_MAIN
// # ifndef GRAPHICS_HPP_MAIN
// # define GRAPHICS_HPP_MAIN
int main(void) {
  graphics_main();
}
// # endif
#endif


#endif