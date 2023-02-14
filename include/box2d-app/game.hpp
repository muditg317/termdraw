#pragma once

#include <termdraw/application.hpp>
#include <termdraw/graphics.hpp>
#include <termdraw/keyboard.hpp>
#include <box2d-app/tools.hpp>

#include <array>
#include <initializer_list>
#include <functional>

#include <box2d/box2d.h>

namespace physics {


class Game : public termdraw::DependentCapability<termdraw::graphics::Graphics,termdraw::keyboard::Keyboard> {
 public:
  enum GameState {
    IDLE,
    START,
    RUNNING,
    OVER
  };
 protected:
  Game(std::array<float, 2> gravity_input, std::initializer_list<physics::GenericContactListener::ContactHandler> contactHandlers);

 protected:
  /**
   * @brief setup for graphics - called directly from main
   */
  virtual void setup(int argc, char *argv[]) = 0;

  /**
   * @brief update the pixel buffer however desired before the next render call
   */
  virtual void update(void) = 0;

  virtual void keyPressHandler(termdraw::keyboard::KeyPressEvent) = 0;

 private:
  physics::GenericContactListener contactListener;

 protected:
  b2Vec2 gravity;
  b2World world;

  GameState state;
  physics::BoundingBox boundingBox;

  virtual void reset(void) = 0;
  virtual void startGame(void) = 0;
  virtual void endGame(void) = 0;
};

} // namespace physics