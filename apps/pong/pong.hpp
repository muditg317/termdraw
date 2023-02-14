#pragma once

#include <termdraw/application.hpp>
#include <termdraw/graphics.hpp>
#include <termdraw/keyboard.hpp>
#include <box2d-app/tools.hpp>

#include <box2d/box2d.h>

enum GameState {
  IDLE,
  START,
  RUNNING,
  OVER
};

class Pong : public termdraw::DependentCapability<termdraw::graphics::Graphics,termdraw::keyboard::Keyboard> {
 public:
  static constexpr const char *NAME = "pong";
  std::string name() const override { return NAME; };

  Pong();

 private:
  b2Vec2 gravity;
  b2World world;

  b2Body *ball;
  b2Body *paddle;
  int score;
  GameState state;
  physics::BoundingBox boundingBox;

 protected:
  /**
   * @brief setup for graphics - called directly from main
   */
  void setup(int argc, char *argv[]);

  /**
   * @brief update the pixel buffer however desired before the next render call
   */
  void update(void);

  void keyPressHandler(termdraw::keyboard::KeyPressEvent);

 private:
  void endGame(void);

  void onBallHit(void);
  void onBallMissed(void);
  physics::GenericContactListener pongContactListener;

  void reset(void);
  void startGame(void);
};


DEFINE_APPLICATION(Pong)