#define GRAPHICS_MAIN
#include <termdraw/graphics.hpp>
#include <termdraw/shapes.hpp>

#include <fmt/format.h>
#include <iostream>
#include <vector>
#include <tuple>

#include <Eigen/Eigen>


Eigen::Vector2f circlePos;
Eigen::Vector2f circleVel;
const Eigen::Vector2f gravity(0,1);
float r = 30;

void reset() {
  circlePos = Eigen::Vector2f(64,30);
  circleVel = Eigen::Vector2f(0,0);
}

void setup() {
  display_size(128,128);
  frameRate(60);
  reset();
}

void update() {
  clean();

  circleVel += gravity;
  circlePos += circleVel;
  if (circlePos.y() >= HEIGHT - r) {
    circleVel.y() *= -0.9;
    circlePos.y() -= 2*(HEIGHT - r);
    circlePos.y() *= -1;
  }
  circle(circlePos,r);
  // printf("Buffer: %d\n", BUFFER[WIDTH/2][HEIGHT/2]);
}

