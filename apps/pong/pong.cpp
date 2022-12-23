#include <pong.hpp>

// #define KEYBOARD_MAIN
#include <termdraw/application.hpp>
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

// #define PIXELS_PER_METER 15.0f
// #define WORLD_WIDTH (WIDTH/PIXELS_PER_METER)
// #define WORLD_HEIGHT (HEIGHT/PIXELS_PER_METER)

#define BALL_SIZE (6.0f / PIXELS_PER_METER)
#define BALL_SPEED (BALL_SIZE * 15.0f)

#define PADDLE_HEIGHT (3.0f / PIXELS_PER_METER)
#define PADDLE_WIDTH (20.0f / PIXELS_PER_METER)
#define PADDLE_Y (WORLD_HEIGHT - PADDLE_HEIGHT*3.0f)
#define PADDLE_SPEED (PADDLE_WIDTH * 0.5f)

Pong::Pong()
  : Application(
      std::make_tuple(
        std::bind(&Pong::setup, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&Pong::update, this)
      ),
      std::make_tuple(
        std::bind(&Pong::keyPressHandler, this, std::placeholders::_1)
      )) {
}

b2Vec2 gravity(0.0f, 0.0f);
b2World world(gravity);

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
physics::BoundingBox boundingBox;

void endGame(void) {
  ball->SetLinearVelocity(b2Vec2_zero);
  state = OVER;
  termdraw::graphics::printf("Press 'r' to restart!");
}

void checkBallHit(b2Contact *contact) {
  if (!physics::isContactBetween(contact, paddle, ball)) {
    return;
  }
  score++;
  termdraw::graphics::printf("Score!! %d\n", score);
}

void checkBallMissed(b2Contact* contact) {
  if (!physics::isContactBetween(contact, boundingBox.bottomWall, ball)) {
    return;
  }
  termdraw::graphics::printf("Game over!! Score: %d\n", score);
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

  physics::makeBoundingBox(&boundingBox, world, WORLD_WIDTH, WORLD_HEIGHT);

  ball = physics::addDynamicCircle(world,
    WORLD_WIDTH/2,
    PADDLE_Y-PADDLE_HEIGHT/2-BALL_SIZE,
    0,0,
    BALL_SIZE);
  paddle = physics::addStaticRect(world,WORLD_WIDTH/2,PADDLE_Y, PADDLE_WIDTH/2, PADDLE_HEIGHT/2);
  

  state = IDLE;
  score = -1;
}

void Pong::keyPressHandler(termdraw::keyboard::KeyPressEvent event) {
  char c = event.c;
  if (c=='q') {
    termdraw::graphics::printf("quit!\n");
    this->quit();
  }
  if (c == 'r') {
    termdraw::graphics::printf("reset!\n");
    reset();
    return;
  }
  if (state == IDLE) {
    if (c == ' ' && state == IDLE) {
      termdraw::graphics::printf("start!\n");
      state = START;
    }
  } else if (state == RUNNING) {
    if (event.specialKey == termdraw::keyboard::LEFT) {
      float newX = std::max(paddle->GetPosition().x - PADDLE_SPEED, PADDLE_WIDTH/2);
      paddle->SetTransform(*new b2Vec2(newX, PADDLE_Y), paddle->GetAngle());
    }
    if (event.specialKey == termdraw::keyboard::RIGHT) {
      float newX = std::min(paddle->GetPosition().x + PADDLE_SPEED, WORLD_WIDTH - PADDLE_WIDTH/2);
      paddle->SetTransform(*new b2Vec2(newX, PADDLE_Y), paddle->GetAngle());
    }
  }
}

void Pong::setup(int argc, char *argv[]) {
  GRAPHICS.display_size_based_on_console(5);
  // display_size(256,128);

  GRAPHICS.frameRate(60);
  world.SetAllowSleeping(true);
  world.SetContinuousPhysics(true);
  world.SetContactListener(&pongContactListener);
  reset();
}

void startGame(void) {
  ball->SetLinearVelocity(physics::randomVelocity(BALL_SPEED));
  state = RUNNING;
}

void Pong::update() {
  GRAPHICS.clean();

  if (state == START) {
    startGame();
  } else if (state == RUNNING) {
    world.Step(DELTA_T_SEC, 8, 1);
  }

  physics::drawCircleBody(ball);
  physics::drawRectBody(paddle);
}

