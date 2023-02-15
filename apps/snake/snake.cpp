#include <snake.hpp>

#include <termdraw/graphics.hpp>
#include <termdraw/keyboard.hpp>
#include <termdraw/shapes.hpp>

#include <box2d-app/tools.hpp>

#include <functional>
#include <cmath>

#include <box2d/box2d.h>

#define SNAKE_MAX_SIZE (6.0f / PIXELS_PER_METER)
#define SNAKE_MIN_SIZE (SNAKE_MAX_SIZE * 0.5f)
#define SNAKE_SPEED (SNAKE_MAX_SIZE * 5.0f)

#define APPLE_SIZE (SNAKE_MAX_SIZE * 0.5f)

Snake::Snake()
  : Game(
      {0.0f, 0.0f},
      {
        physics::ContactListener::ContactHandler(&head, &apple, std::bind(&Snake::onAppleHit, this)),
        physics::ContactListener::ContactHandler(&boundingBox.bottomWall, &head, std::bind(&Snake::onEdgeHit, this)),
        physics::ContactListener::ContactHandler(&boundingBox.topWall, &head, std::bind(&Snake::onEdgeHit, this)),
        physics::ContactListener::ContactHandler(&boundingBox.leftWall, &head, std::bind(&Snake::onEdgeHit, this)),
        physics::ContactListener::ContactHandler(&boundingBox.rightWall, &head, std::bind(&Snake::onEdgeHit, this))
      },
      std::bind(&Snake::checkSelfHit, this, std::placeholders::_1)
    ) {
  // std::cout << "Snake constructor" << std::endl;
}

void Snake::setup(int argc, char *argv[]) {
  APP_GRAPHICS.display_size_based_on_console(5);
  // display_size(256,128);

  APP_GRAPHICS.frameRate(60);
  reset();
}

void Snake::reset() {
  while (world.GetBodyCount() > 0) {
    world.DestroyBody(world.GetBodyList());
  }

  physics::makeBoundingBox(&boundingBox, world, WORLD_WIDTH, WORLD_HEIGHT);

  head = physics::addDynamicCircle(world,
    WORLD_WIDTH/2,
    WORLD_HEIGHT/2,
    0,0,
    SNAKE_MAX_SIZE);
  apple = physics::addStaticRect(world,APPLE_SIZE*1.5,APPLE_SIZE*1.5,APPLE_SIZE,APPLE_SIZE);
  physics::disableBounce(apple);
  // physics::disableBounce(head);
  snake_tail.clear();

  nextApplePosition = nullptr;

  state = physics::Game::GameState::IDLE;
  score = 0;
}

void Snake::startGame(void) {
  head->SetLinearVelocity(physics::randomCardinalVelocity(SNAKE_SPEED));
  randomizeApple();
  state = physics::Game::GameState::RUNNING;
}

void Snake::endGame(void) {
  head->SetLinearVelocity(b2Vec2_zero);
  state = physics::Game::GameState::OVER;
  termdraw::graphics::printf("Press 'r' to restart!");
}

void Snake::keyPressHandler(termdraw::keyboard::KeyPressEvent event) {
  char c = event.c;
  if (c == 'q') {
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
    if (head->GetLinearVelocity().x == 0) {
      if (event.specialKey == termdraw::keyboard::LEFT) {
        // head->SetLinearVelocity(physics::rotateVector(head->GetLinearVelocity(), -M_PI/12));
        head->SetLinearVelocity(*new b2Vec2(-SNAKE_SPEED, 0));
      }
      if (event.specialKey == termdraw::keyboard::RIGHT) {
        // head->SetLinearVelocity(physics::rotateVector(head->GetLinearVelocity(), M_PI/12));
        head->SetLinearVelocity(*new b2Vec2(SNAKE_SPEED, 0));
      }
    } else if (head->GetLinearVelocity().y == 0) {
      if (event.specialKey == termdraw::keyboard::UP) {
        head->SetLinearVelocity(*new b2Vec2(0, -SNAKE_SPEED));
      }
      if (event.specialKey == termdraw::keyboard::DOWN) {
        head->SetLinearVelocity(*new b2Vec2(0, SNAKE_SPEED));
      }
    }
  }
}

void Snake::randomizeApple(void) {
  nextApplePosition = physics::randomPosition(
    SNAKE_MAX_SIZE * 2,
    SNAKE_MAX_SIZE * 2,
    WORLD_WIDTH - SNAKE_MAX_SIZE * 2,
    WORLD_HEIGHT - SNAKE_MAX_SIZE * 2
  );
}

void Snake::onAppleHit(void) {
  score++;
  termdraw::graphics::printf("Score!! %d\n", score);
  randomizeApple();
}

void Snake::onEdgeHit(void) {
  termdraw::graphics::printf("Game over!! Score: %d\n", score);
  endGame();
}

void Snake::checkSelfHit(b2Contact *contact) {
  b2Body *bodyA = contact->GetFixtureA()->GetBody();
  b2Body *bodyB = contact->GetFixtureB()->GetBody();

  if (bodyA == head || bodyB == head) {
    termdraw::graphics::printf("Game over!! Score: %d", score);
    endGame();
  }
}

void Snake::update() {
  APP_GRAPHICS.clean();

  if (state == physics::Game::GameState::START) {
    startGame();
  } else if (state == physics::Game::GameState::RUNNING) {
    world.Step(DELTA_T_SEC, 8, 1);
  }

  if (nextApplePosition != nullptr) {
    apple->SetTransform(*nextApplePosition, apple->GetAngle());
    // termdraw::graphics::printf("Apple at [%4.1f,%4.1f]", apple->GetPosition().x, apple->GetPosition().y);
    nextApplePosition = nullptr;
  }

  physics::drawCircleBody(head);
  physics::drawRectBody(apple);
  // termdraw::graphics::printf("Head @ [%4.1f,%4.1f]\tApple @ [%4.1f,%4.1f]", head->GetPosition().x, head->GetPosition().y, apple->GetPosition().x, apple->GetPosition().y);
}

