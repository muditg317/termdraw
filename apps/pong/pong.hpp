#pragma once

#include <termdraw/application.hpp>
#include <termdraw/graphics.hpp>
#include <termdraw/keyboard.hpp>

class Pong : public termdraw::DependentCapability<termdraw::graphics::Graphics,termdraw::keyboard::Keyboard> {
 public:
  static constexpr const char *NAME = "pong";
  std::string name() const override { return NAME; };

  Pong();

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
};


DEFINE_APPLICATION(Pong)