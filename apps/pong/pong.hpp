#pragma once

#include <termdraw/application.hpp>
#include <termdraw/graphics.hpp>
#include <termdraw/keyboard.hpp>
#include <box2d-app/tools.hpp>
#include <box2d-app/game.hpp>

#include <box2d/box2d.h>

class Pong : public physics::Game {
DEFINE_GAME_FIELDS(Pong)
 private:
  b2Body *ball;
  b2Body *paddle;
  int score;
  
  void onBallHit(void);
  void onBallMissed(void);
};


DEFINE_APPLICATION(Pong)