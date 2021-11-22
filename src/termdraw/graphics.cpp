#include <termdraw/graphics.hpp>

#include <assert.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <string>

static double _frameRate = DEFAULT_FRAME_RATE;
double getFrameRate() {
  return _frameRate;
}

static int _width;
int getWidth() {
  return _width;
}

static int _height;
int getHeight() {
  return _height;
}

TermPix **pixelBuffer;
bool bufferAllocated = false;
TermPix **const getPixelBuffer() {
  assert(bufferAllocated && "Must allocate buffer before reading or writing!");
  // printf("accessed buffer!\n");
  return pixelBuffer;
}

void allocate_buffer(void) {
  pixelBuffer = (TermPix**) calloc(WIDTH, sizeof(TermPix*));
  assert(pixelBuffer != nullptr && "Failed to allocate buffer for terminal drawing session!");
  for (int x = 0; x < WIDTH; x++) {
    pixelBuffer[x] = (TermPix*) calloc(HEIGHT, sizeof(TermPix));
    assert(pixelBuffer[x] != nullptr && "Failed to allocate buffer for terminal drawing session!");
  }
  bufferAllocated = true;
}

static bool dims_set = false;
void display_size(int width, int height) {
  assert(!dims_set && "Display cannot be set twice!");
  _width = width;
  _height = height;
  dims_set = true;

  allocate_buffer();
}

void frameRate(double fr) {
  _frameRate = fr;
  if (fr > MAX_FR_RECOMMENDATION) {
    printf("The provided frame rate %.2f may be too high! Recommended max: %.2f", fr, MAX_FR_RECOMMENDATION);
  }
}

void draw(void) {
  // printf("draw\n");
  char charLine[CONSOLE_WIDTH];
  static char pixelToCharMap[] = {' ','^','_','C'};
  for (int y = 0; y < CONSOLE_HEIGHT; ++y) {
    for (int x = 0; x < CONSOLE_WIDTH; ++x) {
      TermPix b = pixelBuffer[x][2*y];
      TermPix t = pixelBuffer[x][2*y + 1];
      assert(0 <= t);
      assert(t < TERM_PIX_N);
      assert(0 <= b);
      assert(b < TERM_PIX_N);
      charLine[x] = pixelToCharMap[(t<<1)|b];
    }
    fwrite(charLine, CONSOLE_WIDTH, 1, stdout);
    fputc('\n', stdout);
  }
}

void resetCursor(void) {
  printf("\x1b[%dD", CONSOLE_WIDTH);
  printf("\x1b[%dA", CONSOLE_HEIGHT);
}

int graphics_main(void) {
  setup();
  assert(dims_set && "Must set dimensions for terminal drawing session! Call `display_size(width,height)`");
  
  printf("Finished termdraw setup!\n\tScreen: %dx%d\n\tConsole: %dx%d\n\tFrame Rate:%.2f\n", WIDTH, HEIGHT, CONSOLE_WIDTH, CONSOLE_HEIGHT, FRAME_RATE);

  int64_t microsDelay = std::floor(1000000.0/FRAME_RATE);

  std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now();

  while (1) {
    // printf("call update!\n");
    update();
    // printf("call draw!\n");
    draw();
    // printf("call reset!\n");
    resetCursor();

    t += std::chrono::microseconds(microsDelay);
    std::this_thread::sleep_until(t);
  }

  free(pixelBuffer);
  return 0;
}

int main(void) {
  return graphics_main();
}