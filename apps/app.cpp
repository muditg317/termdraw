#define GRAPHICS_MAIN
#include <termdraw/graphics.hpp>
#include <termdraw/shapes.hpp>

#include <fmt/format.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <cmath>

#include <Eigen/Eigen>

#define MAX_CIRCLES 20000
static int circleCount = 5;

Eigen::Vector2f circlePosArr[MAX_CIRCLES];
Eigen::Vector2f circleVelArr[MAX_CIRCLES];
const Eigen::Vector2f gravity(0,0.000141);
float r = 15;

std::random_device rd;
std::mt19937 e2(rd());
std::normal_distribution<> posDist(0,0.5);
std::normal_distribution<> xVelDist(0.15,0.05);
std::uniform_real_distribution<> yVelDist(0.1, 0.05);


void reset() {
  for (int i = 0; i < circleCount; i++) {
    circlePosArr[i] = Eigen::Vector2f(fmod(abs(posDist(e2)), 1)*WIDTH, fmod(abs(posDist(e2)), 1)*HEIGHT);
    circleVelArr[i] = Eigen::Vector2f(xVelDist(e2),yVelDist(e2));
  }
}

void setup(int argc, char *argv[]) {
  std::cout << argc << " args:\t";
  for (int i = 0; i < argc; i++) {
    std::cout << argv[i] << '\t';
  }
  std::cout << std::endl;
  if (argc > 1) {
    circleCount = std::stoi(argv[1]);
  }
  display_size(256,128);
  frameRate(120);
  reset();
}
#define restitution 0.9
#define bounceDamp 1.0
inline void updateCircle(Eigen::Vector2f& pos, Eigen::Vector2f& vel) {
  vel += gravity * DELTA_T;
  pos += vel * DELTA_T;
  if (pos.y() >= HEIGHT - r) {
    vel.y() *= -restitution;
    pos.y() *= bounceDamp-1;
    pos.y() += (2-bounceDamp)*(HEIGHT - r);
  }
  if (pos.y() <= r) {
    vel.y() *= -restitution;
    pos.y() *= bounceDamp-1;
    pos.y() += (2-bounceDamp)*(r);
  }
  if (pos.x() >= WIDTH - r) {
    vel.x() *= -restitution;
    pos.x() *= bounceDamp-1;
    pos.x() += (2-bounceDamp)*(WIDTH - r);
  }
  if (pos.x() <= r) {
    vel.x() *= -restitution;
    pos.x() *= bounceDamp-1;
    pos.x() += (2-bounceDamp)*(r);
  }
  
  circle(pos,r);
}

void update() {
  clean();

  for (int i = 0; i < circleCount; i++) {
    updateCircle(circlePosArr[i], circleVelArr[i]);
  }

  // printf("Buffer: %d\n", BUFFER[WIDTH/2][HEIGHT/2]);
}

