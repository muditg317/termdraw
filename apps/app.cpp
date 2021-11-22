#define GRAPHICS_MAIN
#include <termdraw/graphics.hpp>
#include <termdraw/shapes.hpp>

#include <fmt/format.h>
#include <iostream>
#include <vector>
#include <tuple>


void setup() {
  display_size(128,128);
  frameRate(0.2);
  circle(WIDTH/2, HEIGHT/2, HEIGHT/4);
}

void update() {
  // printf("Buffer: %d\n", BUFFER[WIDTH/2][HEIGHT/2]);
}

