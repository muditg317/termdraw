#pragma once

#include <termdraw/application.hpp>
#include <termdraw/graphics.hpp>
#include <termdraw/keyboard.hpp>
#include <box2d-app/tools.hpp>
#include <box2d-app/game.hpp>

#include <box2d/box2d.h>

class Snake : public physics::Game {
DEFINE_GAME_FIELDS(Snake)
 private:
  std::vector<b2Body *> snake_tail;
  b2Body *head;
  b2Body *apple;
  int score;
  
  b2Vec2 *nextApplePosition;
  void randomizeApple(void);

  void onAppleHit(void);
  void onEdgeHit(void);
  void checkSelfHit(b2Contact *);
};


DEFINE_APPLICATION(Snake)