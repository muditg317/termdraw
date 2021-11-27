#define KEYBOARD_MAIN
#include <termdraw/graphics.hpp>
#include <termdraw/keyboard.hpp>
#include <termdraw/shapes.hpp>

// #include <fmt/format.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <cmath>
#include <chrono>

#include <Eigen/Eigen>
#include <box2d/box2d.h>

#define PIXELS_PER_METER 15
#define WORLD_WIDTH (WIDTH/PIXELS_PER_METER)
#define WORLD_HEIGHT (HEIGHT/PIXELS_PER_METER)

std::random_device rd;
std::mt19937 e2(rd());
std::uniform_real_distribution<> posDist(0,1);
std::normal_distribution<> xVelDist(0,1);
std::normal_distribution<> yVelDist(0,1);

b2Vec2 gravity(0.0f, 0.0f);
b2World world(gravity);

b2AABB worldBounds;

// #define MAX_BODIES 1000
static int circleCount = 5;

// b2Body* bodies[MAX_BODIES] = { 0 };
// static int numBodies = 0;

static float r = 1;

b2Body *addBody(float x, float y, float vx, float vy, b2BodyType type) {
  b2BodyDef bodyDef;
  bodyDef.type = type;
  bodyDef.position.Set(
      x,y
  );
  bodyDef.linearVelocity.Set(
    vx,vy
  );
  // std::cout << "add body at " << x << std::endl;
  // bodyDef.userData = this;
  bodyDef.fixedRotation = true;
  return world.CreateBody(&bodyDef);
}

void addFixtureToBodyWithShape(b2Body *body, b2Shape *shape) {
  b2FixtureDef fixtureDef;
  fixtureDef.shape = shape;
  fixtureDef.density = 1.0f;
  fixtureDef.friction = 0.0f;
  fixtureDef.restitution = 1.0f;
  fixtureDef.restitutionThreshold = 0;
  fixtureDef.filter.categoryBits = 0x02;
  fixtureDef.filter.maskBits = 0xffff;
  body->CreateFixture(&fixtureDef);
}

void addDynamicCircle(float x, float y, float vx, float vy, float rad) {
  b2Body *body = addBody(x,y,vx,vy, b2_dynamicBody);
  // bodies[numBodies++] = body;
  b2CircleShape shape;
  shape.m_radius = r;// * this->getScale();
  // this->createFixture(&shape);
  addFixtureToBodyWithShape(body, &shape);
}

void addStaticRect(float x, float y, float hx, float hy) {
  b2Body *body = addBody(x,y,0,0,b2_staticBody);
  // bodies[numBodies++] = body;
  b2PolygonShape shape;
  shape.SetAsBox(hx,hy);
  // shape.m_radius = r;// * this->getScale();
  // this->createFixture(&shape);
  addFixtureToBodyWithShape(body, &shape);
}

void makeBoundingBox(float width, float height) {
  addStaticRect(width/2,height+1,width/2,1);
  addStaticRect(width/2,-1,width/2,1);
  addStaticRect(width+1,height/2,1,height/2);
  addStaticRect(-1,height/2,1,height/2);
}

static std::chrono::time_point<std::chrono::system_clock> prevTime;
static float deltaT;

void reset() {
  while (world.GetBodyCount() > 0) {
    world.DestroyBody(world.GetBodyList());
  }

  makeBoundingBox(WORLD_WIDTH, WORLD_HEIGHT);

  for (int i = 0; i < circleCount; i++) {
    addDynamicCircle(
      posDist(e2)*WORLD_WIDTH,
      posDist(e2)*WORLD_HEIGHT,
      // i*WORLD_WIDTH/circleCount*0.9 + 0.05*WORLD_WIDTH,
      // 0.1*WORLD_HEIGHT,
      xVelDist(e2),
      yVelDist(e2),
      r);
  }
  prevTime = std::chrono::system_clock::now();

}

bool handler(KeyPressEvent event) {
  char c = event.c;
  if (event.specialKey) {
    graphics_printf("Got special key! %s", special_key_names[event.specialKey]);
    graphics_printf("Modifiers: ctrl:%d|alt:%d|shift:%d",event.ctrl,event.alt,event.shift);
  } else {
    graphics_printf("got %c|%d\n",c,c);
  }
  if (c=='q') {
    graphics_printf("quit!\n");
    return true;
  }
  if (c == 'r') {
    graphics_printf("reset!\n");
    reset();
  }
  return false;
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
  if (argc > 2) {
    r = std::stof(argv[2]) / PIXELS_PER_METER;
  }


  onKeyPress(handler);
  deltaT = DELTA_T_SEC;

  // display_size_based_on_console(5);
  display_size(256,128);

  frameRate(60);
  world.SetAllowSleeping(true);
  world.SetContinuousPhysics(true);
  worldBounds = {.lowerBound = b2Vec2(0,0), .upperBound = b2Vec2(WORLD_WIDTH,WORLD_HEIGHT)};
  reset();
}

class FixtureDrawer : public b2QueryCallback {
  public:
    bool ReportFixture(b2Fixture* fixture) {
      if (fixture->GetShape()->GetType() != b2Shape::Type::e_circle) {
        return true;
      }
      b2Vec2 pos = fixture->GetBody()->GetPosition();
      float rad = fixture->GetShape()->m_radius;
      circle(pos.x*PIXELS_PER_METER,pos.y*PIXELS_PER_METER, rad*PIXELS_PER_METER);
      return true;
    }
};

FixtureDrawer fixtureCallback;

void update() {
  clean();

  auto currTime = std::chrono::system_clock::now();
  // deltaT = (currTime - prevTime).count() / 1000000000.0;
  prevTime = currTime;

  // std::cout << "delta t: " << deltaT << std::endl;

  world.Step(deltaT, 8, 1);

  world.QueryAABB(&fixtureCallback, worldBounds);

  // for (b2Body *body = world.GetBodyList(), *end = body + world.GetBodyCount(); body < end; body++) {
  //   b2Vec2 pos = body->GetPosition();
  //   // if (body->GetFixtureList()->GetShape()->GetType() == b2Shape::Type::e_circle) {
  //     // std::cout << "found circle" << std::endl;
  //     // float rad = body->GetFixtureList()->GetShape()->m_radius;
  //   circle(pos.x, pos.y, r);
  //   // } else {
  //   //   std::cout << "not a circle!" << std::endl;
  //   // }
  // }
}

