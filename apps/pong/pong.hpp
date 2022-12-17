#ifndef PONG_HPP
#define PONG_HPP

#include <termdraw/keyboard.hpp>

class Pong : public KeyboardGraphicsApplication {
 protected:
  /**
   * @brief setup for graphics - called directly from main
   */
  void setup(int argc, char *argv[]);

  /**
   * @brief update the pixel buffer however desired before the next render call
   */
  void update(void);

  void keyPressHandler(KeyPressEvent);
};

#endif