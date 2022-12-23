#pragma once

#include <random>

#include <box2d/box2d.h>

#define PIXELS_PER_METER 15.0f
#define METER_TO_PIXELS(m) (m*PIXELS_PER_METER)
#define PIXELS_TO_METER(p) (p/PIXELS_PER_METER)

#define WORLD_WIDTH PIXELS_TO_METER(WIDTH)
#define WORLD_HEIGHT PIXELS_TO_METER(HEIGHT)

namespace physics {

struct BoundingBox {
  b2Body *bottomWall;
  b2Body *topWall;
  b2Body *leftWall;
  b2Body *rightWall;
};

b2Body *addBody(b2World &world, float x, float y, float vx, float vy, b2BodyType type);

void addFixtureToBodyWithShape(b2Body *body, b2Shape *shape);

b2Body *addDynamicCircle(b2World &world, float x, float y, float vx, float vy, float rad);

b2Body *addStaticRect(b2World &world, float x, float y, float hx, float hy);

void makeBoundingBox(BoundingBox *out_boundingBox, b2World &world, float width, float height);

/**
 * @brief creates a 2d vector with random angle and input speed
 * 
 * @param speed the magnitude of the vector (default unit vector)
 * @return b2Vec2 a 2d vector with random angle and desired magnitude
 */
inline b2Vec2 randomVelocity(int speed = 1) {
  float angle = -((float) random() / (float) RAND_MAX) * M_PI;
  return speed * *new b2Vec2(std::cos(angle),std::sin(angle));
}

bool isContactBetween(b2Contact *contact, b2Body *body1, b2Body *body2);

void drawCircleBody(b2Body *circleBody);

void drawRectBody(b2Body *rectBody);

} // namespace physics