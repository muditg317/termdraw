#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

enum SpecialKey {
  NONE = 0,
  ESC,
  INSERT,
  DELETE,
  UP,
  DOWN,
  LEFT,
  RIGHT,
  PAGE_UP,
  PAGE_DOWN,
  HOME,
  END,
};

typedef struct KeyPressEvent {
  int numBytes;
  char *allBytes;
  char c;
  bool shift;
  bool ctrl;
  bool alt;
  SpecialKey specialKey;

  KeyPressEvent(int numBytesToRead);
  void processSpecialBytes(void);

  ~KeyPressEvent();

} KeyPressEvent;

typedef bool keyPressHandler(KeyPressEvent);

/**
 * @brief register a key pres handler for the graphics session
 * should return boolean (true to terminate program, false otherwise)
 */
void onKeyPress(keyPressHandler *);

void keyboard_preloop(void);
void keyboard_loop(void);
void keyboard_finish(int signum=0);

int keyboard_main(int argc, char *argv[]);

#ifdef KEYBOARD_MAIN
int main(int argc, char *argv[]) {
  keyboard_main(argc, argv);
}
#endif


#endif