#include <termdraw/graphics.hpp>
#include <termdraw/shapes.hpp>

#include <Eigen/Eigen>
#include <iostream>

void circle(float x, float y, float r) {
  return circle(Eigen::Vector2f(x,y), r);
}

void circle(Eigen::Vector2f center, float radius) {
  // std::cout  << "circle at <" << center.x() << ',' << center.y() << "> | r=" << radius << std::endl;
  Eigen::Vector2i topLeft = (center - Eigen::Vector2f(radius, radius)).array().floor().cast<int>();
  Eigen::Vector2i bottomRight = (center + Eigen::Vector2f(radius, radius)).array().ceil().cast<int>();

  for (int y = topLeft.y(); y <= bottomRight.y(); ++y) {
      for (int x = topLeft.x(); x <= bottomRight.x(); ++x) {
          Eigen::Vector2f coordCenter = Eigen::Vector2f(x,y) + Eigen::Vector2f(0.5, 0.5);
          Eigen::Vector2f delta = center - coordCenter;
          if (delta.squaredNorm() <= radius*radius) {
              if (0 <= x && x < WIDTH && 0 <= y && y < HEIGHT) {
                  BUFFER[x][y] = ON;
              }
          }
      }
  }
}