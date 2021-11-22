#include <linalg/vectors.hpp>
// #define GRAPHICS_IMPL_ONLY
#include <termdraw/graphics.hpp>

#include <fmt/format.h>
#include <iostream>
#include <vector>
#include <tuple>

void setup() {
  display_size(32,32);
  frameRate(0.2);
  BUFFER[WIDTH/2][HEIGHT/2] = ON;
  BUFFER[WIDTH/2+1][HEIGHT/2] = ON;
  BUFFER[WIDTH/2][HEIGHT/2+1] = ON;
  BUFFER[WIDTH/2+1][HEIGHT/2+1] = ON;
}

void update() {
  // printf("Buffer: %d\n", BUFFER[WIDTH/2][HEIGHT/2]);
}

// int main() {
//     printf("Hello world!\n");

//     auto vec = as_vec<int, 5>(5);
//     vec_print(vec, "test");

//     std::cout << "⠂" << std::endl;

//     return 0;
// }
