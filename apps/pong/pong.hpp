#pragma once

#include <termdraw/application.hpp>
#include <termdraw/graphics.hpp>
#include <termdraw/keyboard.hpp>
#include <box2d-app/tools.hpp>
#include <box2d-app/game.hpp>

#include <box2d/box2d.h>

class Pong : public physics::Game {
 public:
  static constexpr const char *NAME = "pong";
  std::string name() const override { return NAME; };

  Pong();

 private:

  b2Body *ball;
  b2Body *paddle;
  int score;

 private:
  void setup(int argc, char *argv[]) override;
  void update(void) override;
  void keyPressHandler(termdraw::keyboard::KeyPressEvent) override;

 private:
  void reset(void) override;
  void startGame(void) override;
  void endGame(void) override;

  void onBallHit(void);
  void onBallMissed(void);
};


DEFINE_APPLICATION(Pong)