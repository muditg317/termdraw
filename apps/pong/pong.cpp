#define KEYBOARD_MAIN
#include <termdraw/graphics.hpp>
#include <termdraw/keyboard.hpp>
#include <termdraw/shapes.hpp>

#include <box2d-app/tools.hpp>

// #include <fmt/format.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <cmath>
#include <chrono>

#include <Eigen/Eigen>
#include <box2d/box2d.h>

#define PIXELS_PER_METER 15.0f
#define WORLD_WIDTH (WIDTH/PIXELS_PER_METER)
#define WORLD_HEIGHT (HEIGHT/PIXELS_PER_METER)

#define BALL_SIZE (6.0f / PIXELS_PER_METER)
#define BALL_SPEED (BALL_SIZE * 15.0f)

#define PADDLE_HEIGHT (3.0f / PIXELS_PER_METER)
#define PADDLE_WIDTH (20.0f / PIXELS_PER_METER)
#define PADDLE_Y (WORLD_HEIGHT - PADDLE_HEIGHT*3.0f)
#define PADDLE_SPEED (PADDLE_WIDTH * 0.5f)

b2Vec2 gravity(0.0f, 0.0f);
b2World world(gravity);

// b2AABB worldBounds;

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

// void addFixtureToBodyWithShape(b2Body *body, b2Shape *shape) {
//   b2FixtureDef fixtureDef;
//   fixtureDef.shape = shape;
//   fixtureDef.density = 1.0f;
//   fixtureDef.friction = 0.0f;
//   fixtureDef.restitution = 1.0f;
//   fixtureDef.restitutionThreshold = 0;
//   fixtureDef.filter.categoryBits = 0x02;
//   fixtureDef.filter.maskBits = 0xffff;
//   body->CreateFixture(&fixtureDef);
// }

b2Body *addDynamicCircle(float x, float y, float vx, float vy, float rad) {
  b2Body *body = addBody(x,y,vx,vy, b2_dynamicBody);
  // bodies[numBodies++] = body;
  b2CircleShape shape;
  shape.m_radius = rad;// * this->getScale();
  // this->createFixture(&shape);
  addFixtureToBodyWithShape(body, &shape);
  return body;
}

b2Body *addStaticRect(float x, float y, float hx, float hy) {
  b2Body *body = addBody(x,y,0,0,b2_staticBody);
  // bodies[numBodies++] = body;
  b2PolygonShape shape;
  shape.SetAsBox(hx,hy);
  // shape.m_radius = r;// * this->getScale();
  // this->createFixture(&shape);
  addFixtureToBodyWithShape(body, &shape);
  return body;
}


void makeBoundingBox(BoundingBox *out_boundingBox, float width, float height) {
  out_boundingBox->bottomWall = addStaticRect(width/2,height+1,width/2,1);
  // topWall
  out_boundingBox->topWall = addStaticRect(width/2,-1,width/2,1);
  out_boundingBox->rightWall = addStaticRect(width+1,height/2,1,height/2);
  out_boundingBox->leftWall = addStaticRect(-1,height/2,1,height/2);
}

enum GameState {
  IDLE,
  START,
  RUNNING,
  OVER
};

static b2Body *ball;
static b2Body *paddle;
static int score;
static GameState state;
BoundingBox boundingBox;

void endGame(void) {
  ball->SetLinearVelocity(b2Vec2_zero);
  state = OVER;
  graphics_printf("Press 'r' to restart!");
}

void checkBallHit(b2Contact *contact) {
  if (!isContactBetween(contact, paddle, ball)) {
    return;
  }
  score++;
  graphics_printf("Score!! %d\n", score);
}

void checkBallMissed(b2Contact* contact) {
  if (!isContactBetween(contact, boundingBox.bottomWall, ball)) {
    return;
  }
  graphics_printf("Game over!! Score: %d\n", score);
  endGame();
}

class PongContactListener : public b2ContactListener {
  void BeginContact(b2Contact* contact) {}

  void EndContact(b2Contact* contact) {
    checkBallHit(contact);
    checkBallMissed(contact);
  }
};

PongContactListener pongContactListener;

void reset() {
  while (world.GetBodyCount() > 0) {
    world.DestroyBody(world.GetBodyList());
  }

  makeBoundingBox(&boundingBox, WORLD_WIDTH, WORLD_HEIGHT);

  ball = addDynamicCircle(
    WORLD_WIDTH/2,
    PADDLE_Y-PADDLE_HEIGHT/2-BALL_SIZE,
    0,0,
    BALL_SIZE);
  paddle = addStaticRect(WORLD_WIDTH/2,PADDLE_Y, PADDLE_WIDTH/2, PADDLE_HEIGHT/2);
  
  // graphics_printf("Make paddle at <%.2f,%.2f> with halves <%.2f,%.2f>",WORLD_WIDTH/2,PADDLE_Y, PADDLE_WIDTH/2, PADDLE_HEIGHT/2);

  state = IDLE;
  score = -1;
}

bool onKeyDown(KeyPressEvent event) {
  char c = event.c;
  // if (event.specialKey)
  //   graphics_printf("Got special key! %s", special_key_names[event.specialKey]);
  // else
  //   graphics_printf("got %c|%d\n",c,c);
  // if (event.modified) graphics_printf("Modifiers: ctrl:%d|alt:%d|shift:%d",event.ctrl,event.alt,event.shift);
  if (c=='q') {
    graphics_printf("quit!\n");
    return true;
  }
  if (c == 'r') {
    graphics_printf("reset!\n");
    reset();
    return false;
  }
  if (state == IDLE) {
    if (c == ' ' && state == IDLE) {
      graphics_printf("start!\n");
      state = START;
    }
  } else if (state == RUNNING) {
    if (event.specialKey == LEFT) {
      float nexX = std::max(paddle->GetPosition().x - PADDLE_SPEED, PADDLE_WIDTH/2);
      paddle->SetTransform(*new b2Vec2(nexX, PADDLE_Y), paddle->GetAngle());
    }
    if (event.specialKey == RIGHT) {
      float nexX = std::min(paddle->GetPosition().x + PADDLE_SPEED, WORLD_WIDTH - PADDLE_WIDTH/2);
      paddle->SetTransform(*new b2Vec2(nexX, PADDLE_Y), paddle->GetAngle());
    }
  }
  return false;
}

void setup(int argc, char *argv[]) {
  registerKeyPressHandler(onKeyDown);

  display_size_based_on_console(5);
  // display_size(256,128);

  frameRate(60);
  world.SetAllowSleeping(true);
  world.SetContinuousPhysics(true);
  world.SetContactListener(&pongContactListener);
  // worldBounds = {.lowerBound = b2Vec2(0,0), .upperBound = b2Vec2(WORLD_WIDTH,WORLD_HEIGHT)};
  reset();
}

void startGame(void) {
  ball->SetLinearVelocity(randomVelocity(BALL_SPEED));
  state = RUNNING;
}

void update() {
  clean();

  if (state == START) {
    startGame();
  } else if (state == RUNNING) {
    world.Step(DELTA_T_SEC, 8, 1);
  }

  drawCircleBody(ball);
  drawRectBody(paddle);
}