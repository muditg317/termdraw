#include <termdraw/graphics.hpp>
#include <termdraw/keyboard.hpp>

#include <unistd.h>
#include <csignal>
#include <sys/ioctl.h>
#include <cctype>
#include <termios.h>
#include <functional>
#include <stdexcept>

KeyPressEvent::KeyPressEvent(char byte) :
  c(byte),
  numBytes(1),
  specialKey(NONE),
  modified(false),
  ctrl(false),
  alt(false),
  shift(false) {};

KeyPressEvent::KeyPressEvent(char *inputBytes, int incomingByteCount) :
  numBytes(incomingByteCount),
  c(0),
  specialKey(NONE),
  modified(false),
  ctrl(false),
  alt(false),
  shift(false) {
  // graphics_printf("read %d bytes from buffer!", numBytes);
  for (int i = 0; i < numBytes; i++) {
    allBytes[i] = inputBytes[i];
  }
  allBytes[numBytes] = 0;
  // graphics_printf("Bytes (%d) read: %c\n",numBytes,*allBytes);
  if (*allBytes == '\e') {
    processSpecialBytes();
  }
  // c = 0;//allBytes[numBytes-1];
}

void KeyPressEvent::processSpecialBytes(void) {
  assert(*allBytes == '\e' && "Cannot process special bytes for non control sequence bytes!");
  if (numBytes == 1) { // just the '\e'
    specialKey = ESC;
    // graphics_printf("Got ESC!\n");
    return;
  }
  if (allBytes[1] == '[') { // control sequence starts with '['
    switch (allBytes[numBytes-1]) { // switch on the last character of the event
      case 'A':
        specialKey = UP;
        break;
      case 'B':
        specialKey = DOWN;
        break;
      case 'C':
        specialKey = RIGHT;
        break;
      case 'D':
        specialKey = LEFT;
        break;
      case 'H':
        specialKey = HOME;
        break;
      case 'F':
        specialKey = END;
        break;
      case '~':
        switch (allBytes[2]) { // switch on 3rd character
          case '2':
            specialKey = INSERT;
            break;
          case '3':
            specialKey = DELETE;
            break;
          case '5':
            specialKey = PAGE_UP;
            break;
          case '6':
            specialKey = PAGE_DOWN;
            break;
        }
        break;
    }
    if (allBytes[3] == ';') { // has modifiers
      modified = true;
      //     +   +shift - 50 == 110010
      //     +alt+      - 51 == 110011
      //     +alt+shift - 52 == 110100
      // ctrl           - 53 == 110101
      // ctrl+   +shift - 54 == 110110
      // ctrl+alt+      - 55 == 110111
      // ctrl+alt+shift - 56 == 111000
      // shift --       bit0 ==      0
      // alt   --       bit0 == bit1
      // ctrl  --       bit4 != bit3
      shift = (allBytes[4] & 0b1) == 0;
      alt = (allBytes[4] & 0b1) == ((allBytes[4] & 0b10) >> 1);
      ctrl = (((allBytes[4] & 0b1000) >> 3) != ((allBytes[4] & 0b100) >> 2)) && allBytes[4] != 52;
    }
  } else if (std::islower(allBytes[1])) {
    modified = true;
    alt = true;
    c = allBytes[1];
    // specialKey = ALT_LOWERCASE;
  }
}

KeyboardGraphicsApplication::KeyboardGraphicsApplication()
    : GraphicsApplication() {
  this->registerPreloop(std::bind(&KeyboardGraphicsApplication::keyboard_preloop, this, std::placeholders::_1, std::placeholders::_2));
  this->registerLoop(std::bind(&KeyboardGraphicsApplication::keyboard_loop, this));
  this->registerFinish(std::bind(&KeyboardGraphicsApplication::keyboard_finish, this, std::placeholders::_1));
}

KeyboardGraphicsApplication::~KeyboardGraphicsApplication() {}

// void KeyboardGraphicsApplication::setupRegistryFuncs(void) {
//   GraphicsApplication::setupRegistryFuncs();
// }


static struct termios oldSettings, newSettings;

void KeyboardGraphicsApplication::keyboard_preloop(int argc, char *argv[]) {
  tcgetattr(STDIN_FILENO, &oldSettings );
  newSettings = oldSettings;
  newSettings.c_lflag &= (~ICANON & ~ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newSettings );
}


void KeyboardGraphicsApplication::keyboard_loop(void) {
  static int n = 0;
  while (ioctl(STDIN_FILENO, FIONREAD, &n) == 0 && n > 0) {
    assert(n <= MAX_INPUT_BYTES && "Cannot buffer more than MAX_INPUT_BYTES characters! must input slower");
    static char buffer[MAX_INPUT_BYTES];
    for (int i = 0; i < n; i++) {
      buffer[i] = fgetc(stdin);
      // graphics_printf("read byte: %c|%d\n", buffer[i], buffer[i]);
    }
    int controlSeqStart = -1;
    int controlSeqEnd = -1;

    for (int i = 0; i < n; i++) {
      if (controlSeqStart != -1) {
        if (controlSeqStart == i-1 && buffer[i] != '[' && buffer[i] != 'O' && !std::islower(buffer[i])) {
          // just started
          // don't have '[' -- most control sequences
          // don't have 'O' -- some F-keys
          // don't have lowercase -- alt+letter
          controlSeqEnd = controlSeqStart;
        } else if (std::isupper(buffer[i]) || buffer[i] == '~') { // ending with a capital letter or ~
          controlSeqEnd = i;
        }
        if (controlSeqEnd != -1) {
          // graphics_printf("end control sequence! %d", controlSeqEnd);
          this->keyPressHandler(KeyPressEvent(buffer + controlSeqStart, controlSeqEnd - controlSeqStart + 1));
          controlSeqStart = -1;
          controlSeqEnd = -1;
        }
      } else if (buffer[i] == '\e') {
        controlSeqStart = i;
        // graphics_printf("start control sequence! %d", i);
      } else if (controlSeqStart == -1) {
        this->keyPressHandler(KeyPressEvent(buffer[i]));
      }
    }
    if (controlSeqStart != -1) {
      controlSeqEnd = n-1;
      this->keyPressHandler(KeyPressEvent(buffer + controlSeqStart, controlSeqEnd - controlSeqStart + 1));
    }
  }
}

void KeyboardGraphicsApplication::keyboard_finish(int signum) {
  graphics_printf("Reset terminal settings!");
  tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings );
}