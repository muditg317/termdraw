#include <pong.hpp>

#include <termdraw/application.hpp>
#include <termdraw/capability.hpp>
#include <termdraw/graphics.hpp>
#include <termdraw/keyboard.hpp>
#include <termdraw/shapes.hpp>

#include <box2d-app/tools.hpp>

// #include <fmt/format.h>
#include <iostream>
#include <functional>
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
  : Game(
      {0.0f, 0.0f},
      {
        physics::GenericContactListener::ContactHandler(&paddle, &ball, std::bind(&Pong::onBallHit, this)),
        physics::GenericContactListener::ContactHandler(&boundingBox.bottomWall, &ball, std::bind(&Pong::onBallMissed, this))
      }
    ) {
  // std::cout << "Pong constructor" << std::endl;
}

void Pong::setup(int argc, char *argv[]) {
  APP_GRAPHICS.display_size_based_on_console(5);
  // display_size(256,128);

  APP_GRAPHICS.frameRate(60);
  reset();
}

void Pong::reset() {
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
  

  state = physics::Game::GameState::IDLE;
  score = -1;
}

void Pong::startGame(void) {
  ball->SetLinearVelocity(physics::randomVelocity(BALL_SPEED));
  state = physics::Game::GameState::RUNNING;
}

void Pong::endGame(void) {
  ball->SetLinearVelocity(b2Vec2_zero);
  state = physics::Game::GameState::OVER;
  termdraw::graphics::printf("Press 'r' to restart!");
}

void Pong::keyPressHandler(termdraw::keyboard::KeyPressEvent event) {
  char c = event.c;
  if (c=='q') {
    termdraw::graphics::printf("quit!\n");
    APP->quit();
  }
  if (c == 'r') {
    termdraw::graphics::printf("reset!\n");
    reset();
    return;
  }
  if (state == physics::Game::GameState::IDLE) {
    if (c == ' ' && state == physics::Game::GameState::IDLE) {
      termdraw::graphics::printf("start!\n");
      state = physics::Game::GameState::START;
    }
  } else if (state == physics::Game::GameState::RUNNING) {
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

void Pong::onBallHit(void) {
  // if (!physics::isContactBetween(contact, paddle, ball)) {
  //   return;
  // }
  score++;
  termdraw::graphics::printf("Score!! %d\n", score);
}

void Pong::onBallMissed(void) {
  // if (!physics::isContactBetween(contact, boundingBox.bottomWall, ball)) {
  //   return;
  // }
  termdraw::graphics::printf("Game over!! Score: %d\n", score);
  endGame();
}

void Pong::update() {
  APP_GRAPHICS.clean();

  if (state == physics::Game::GameState::START) {
    startGame();
  } else if (state == physics::Game::GameState::RUNNING) {
    world.Step(DELTA_T_SEC, 8, 1);
  }

  physics::drawCircleBody(ball);
  physics::drawRectBody(paddle);
}

