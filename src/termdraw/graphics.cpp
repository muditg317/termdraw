#include <termdraw/graphics.hpp>

#include <cassert>
#include <iostream>
#include <cstring>
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
char *consoleBuffer;

bool pixelBufferAllocated = false;
TermPix **const getPixelBuffer() {
  assert(pixelBufferAllocated && "Must allocate buffer before reading or writing!");
  // printf("accessed buffer!\n");
  return pixelBuffer;
}

void allocate_buffers(void) {
  pixelBuffer = (TermPix**) calloc(WIDTH, sizeof(TermPix*));
  assert(pixelBuffer != nullptr && "Failed to allocate pixel buffer for terminal drawing session!");
  for (int x = 0; x < WIDTH; x++) {
    pixelBuffer[x] = (TermPix*) calloc(HEIGHT, sizeof(TermPix));
    assert(pixelBuffer[x] != nullptr && "Failed to allocate pixel buffer for terminal drawing session!");
  }
  pixelBufferAllocated = true;

  consoleBuffer = (char *) calloc(CONSOLE_HEIGHT, sizeof(char) * CONSOLE_LINE_SIZE);
  assert(consoleBuffer != nullptr && "Failed to allocate console buffer for terminal drawing session!");
}

static bool dims_set = false;
void display_size(int width, int height) {
  assert(!dims_set && "Display cannot be set twice!");
  _width = width;
  _height = height;
  dims_set = true;

  allocate_buffers();
}

void frameRate(double fr) {
  _frameRate = fr;
  if (fr > MAX_FR_RECOMMENDATION) {
    printf("The provided frame rate %.2f may be too high! Recommended max: %.2f", fr, MAX_FR_RECOMMENDATION);
  }
}

inline char pixelsToBraille_offset1(TermPix r0c0, TermPix r0c1,
                             TermPix r1c0, TermPix r1c1,
                             TermPix r2c0, TermPix r2c1,
                             TermPix r3c0, TermPix r3c1) {
  return r3c0 + (r3c1 << 1);
}

inline char pixelsToBraille_offset2(TermPix r0c0, TermPix r0c1,
                             TermPix r1c0, TermPix r1c1,
                             TermPix r2c0, TermPix r2c1,
                             TermPix r3c0, TermPix r3c1) {
  return r0c0 + (r1c0 << 1) + (r2c0 << 2) + (r0c1 << 3) + (r1c1 << 4) + (r2c1 << 5);
}

inline void draw(void) {
  // printf("draw\n");
  // char charLine[CONSOLE_HEIGHT * CONSOLE_LINE_SIZE]; // each braille character is 3 bytes
  static char baseBraille[] = "\u2800";
  for (int y = 0; y < CONSOLE_HEIGHT; ++y) {
    for (int x = 0; x < CONSOLE_WIDTH; ++x) {
      TermPix r0c0 = pixelBuffer[WIDTH_SCALE*x][HEIGHT_SCALE*y];
      TermPix r0c1 = pixelBuffer[WIDTH_SCALE*x+1][HEIGHT_SCALE*y];
      TermPix r1c0 = pixelBuffer[WIDTH_SCALE*x][HEIGHT_SCALE*y+1];
      TermPix r1c1 = pixelBuffer[WIDTH_SCALE*x+1][HEIGHT_SCALE*y+1];
      TermPix r2c0 = pixelBuffer[WIDTH_SCALE*x][HEIGHT_SCALE*y+2];
      TermPix r2c1 = pixelBuffer[WIDTH_SCALE*x+1][HEIGHT_SCALE*y+2];
      TermPix r3c0 = pixelBuffer[WIDTH_SCALE*x][HEIGHT_SCALE*y+3];
      TermPix r3c1 = pixelBuffer[WIDTH_SCALE*x+1][HEIGHT_SCALE*y+3];
      // TODO: validate pixel values if i want

      char offset1 = pixelsToBraille_offset1(r0c0,r0c1,r1c0,r1c1,r2c0,r2c1,r3c0,r3c1);
      char offset2 = pixelsToBraille_offset2(r0c0,r0c1,r1c0,r1c1,r2c0,r2c1,r3c0,r3c1);

      consoleBuffer[CONSOLE_LINE_SIZE*y + BYTES_PER_CONSOLE_CHAR*x+0] = baseBraille[0];
      consoleBuffer[CONSOLE_LINE_SIZE*y + BYTES_PER_CONSOLE_CHAR*x+1] = baseBraille[1] + offset1;
      consoleBuffer[CONSOLE_LINE_SIZE*y + BYTES_PER_CONSOLE_CHAR*x+2] = baseBraille[2] + offset2;
    }
    consoleBuffer[CONSOLE_LINE_SIZE*(y+1) - 1] = '\n';
  }
  fwrite(consoleBuffer, CONSOLE_LINE_SIZE, CONSOLE_HEIGHT, stdout);
}

void clean(void) {
  for (int x = 0; x < WIDTH; x++) {
    memset(pixelBuffer[x], 0, sizeof(TermPix) * HEIGHT);
  }
}

inline void resetCursor(void) {
  printf("\x1b[%dD", CONSOLE_WIDTH);
  printf("\x1b[%dA", CONSOLE_HEIGHT);
}

int graphics_main(void) {
  setup();
  assert(dims_set && "Must set dimensions for terminal drawing session! Call `display_size(width,height)`");
  
  printf("Finished termdraw setup!\n\tScreen:\t%3dx%3d\n\tConsole:\t%3dx%3d\n\tFrame Rate:\t%.2f\n", WIDTH, HEIGHT, CONSOLE_WIDTH, CONSOLE_HEIGHT, FRAME_RATE);

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