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
const Eigen::Vector2f gravity(0,0.0001);
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
#define restitution 1.0
#define bounceDamp 1.0
void update() {
  clean();

  circleVel += gravity * DELTA_T;
  circlePos += circleVel * DELTA_T;
  if (circlePos.y() >= HEIGHT - r) {
    circleVel.y() *= -restitution;
    circlePos.y() *= bounceDamp-1;
    circlePos.y() += (2-bounceDamp)*(HEIGHT - r);
  }
  circle(circlePos,r);
  // printf("Buffer: %d\n", BUFFER[WIDTH/2][HEIGHT/2]);
}

