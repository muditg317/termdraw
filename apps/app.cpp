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
const Eigen::Vector2f gravity(0,0.00015);
float r = 10;

void reset() {
  circlePos = Eigen::Vector2f(64,30);
  circleVel = Eigen::Vector2f(0.1,0);
}

void setup() {
  display_size(256,128);
  frameRate(100);
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
  if (circlePos.x() >= WIDTH - r) {
    circleVel.x() *= -restitution;
    circlePos.x() *= bounceDamp-1;
    circlePos.x() += (2-bounceDamp)*(WIDTH - r);
  }
  if (circlePos.x() <= r) {
    circleVel.x() *= -restitution;
    circlePos.x() *= bounceDamp-1;
    circlePos.x() += (2-bounceDamp)*(r);
  }
  circle(circlePos,r);
  // printf("Buffer: %d\n", BUFFER[WIDTH/2][HEIGHT/2]);
}

