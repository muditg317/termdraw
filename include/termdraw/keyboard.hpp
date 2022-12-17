#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include <termdraw/graphics.hpp>

#include <cstddef>

#define MAX_INPUT_BYTES 10

#define SPECIAL_KEYS \
X(NONE, "NONE") \
X(ESC, "ESC") \
X(INSERT, "INSERT") \
X(DELETE, "DELETE") \
X(UP, "UP") \
X(DOWN, "DOWN") \
X(LEFT, "LEFT") \
X(RIGHT, "RIGHT") \
X(PAGE_UP, "PAGE_UP") \
X(PAGE_DOWN, "PAGE_DOWN") \
X(HOME, "HOME") \
X(END, "END") \
X(ALT_LOWERCASE, "ALT_LOWERCASE")

#define X(key, name) key,
enum SpecialKey : size_t {
  SPECIAL_KEYS
};
#undef X

#define X(key, name) name,
static char const *special_key_names[] = {
  SPECIAL_KEYS
};
#undef X

typedef struct KeyPressEvent {
  int numBytes;
  char allBytes[MAX_INPUT_BYTES];
  char c;
  bool modified;
  bool shift;
  bool ctrl;
  bool alt;
  SpecialKey specialKey;

  KeyPressEvent(char byte);
  KeyPressEvent(char *inputBytes, int incomingByteCount);
  void processSpecialBytes(void);

} KeyPressEvent;


class KeyboardGraphicsApplication : public GraphicsApplication {
 public:
  KeyboardGraphicsApplication();
  virtual ~KeyboardGraphicsApplication();

 protected:
  virtual void keyPressHandler(KeyPressEvent) = 0;

 private:
  void keyboard_preloop(int argc, char *argv[]);
  void keyboard_loop(void);
  void keyboard_finish(int signum=0);

  int keyboard_main(int argc, char *argv[]);
};


#endif