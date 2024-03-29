#pragma once

#include <termdraw/application.hpp>
#include <termdraw/capability.hpp>

#include <cstddef>
#include <functional>

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

namespace termdraw {

namespace keyboard {

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


class Keyboard : public Capability<> {

  typedef std::function<void(KeyPressEvent)> keyPressHandlerFunc_t;

 public:
  static constexpr const char *NAME = "keyboard";
  std::string name() const override { return NAME; };


  Keyboard(keyPressHandlerFunc_t);
  ~Keyboard();
 protected:
  // virtual void keyPressHandler(KeyPressEvent) = 0;
  keyPressHandlerFunc_t keyPressHandler;

 private:
  CapabilityBase::preloopFunc preloop override;
  CapabilityBase::loopFunc loop override;
  CapabilityBase::finishFunc finish override;

};

} // namespace keyboard

} // namespace termdraw
