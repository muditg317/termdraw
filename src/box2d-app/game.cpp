#include <box2d-app/game.hpp>


#include <array>
#include <initializer_list>
#include <functional>

namespace physics {

Game::Game(std::array<float, 2> gravity_input, std::initializer_list<ContactListener::ContactHandler> contactHandlers, ContactListener::fallbackHandler_t fallbackHandler)
  : DependentCapability(
      std::make_tuple(
        std::bind(&Game::setup, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&Game::update, this)
      ),
      std::make_tuple(
        std::bind(&Game::keyPressHandler, this, std::placeholders::_1)
      )
    ),
    gravity(gravity_input[0], gravity_input[1]),
    world(gravity),
    contactListener(contactHandlers, fallbackHandler) {
  world.SetAllowSleeping(true);
  world.SetContinuousPhysics(true);
  world.SetContactListener(&contactListener);
}

Game::Game(std::array<float, 2> gravity_input, std::initializer_list<ContactListener::ContactHandler> contactHandlers)
  : Game(gravity_input, contactHandlers, ContactListener::fallbackHandlerDefault) {}

} // namespace physics