#include <termdraw/graphics.hpp>
#include <termdraw/keyboard.hpp>

#include <unistd.h>
#include <csignal>
#include <sys/ioctl.h>
#include <cctype>

#define USE_KEYBOARD

#define NB_DISABLE 0
#define NB_ENABLE 1


KeyPressEvent::KeyPressEvent(char byte) :
  c(byte),
  numBytes(1),
  specialKey(NONE),
  ctrl(false),
  alt(false),
  shift(false) {};

KeyPressEvent::KeyPressEvent(char *inputBytes, int incomingByteCount) :
  numBytes(incomingByteCount),
  specialKey(NONE),
  ctrl(false),
  alt(false),
  shift(false) {
  // graphics_printf("read %d bytes from buffer!", numBytes);
  // allBytes = (char *) malloc(sizeof(char) * numBytes + 1);
  // assert (allBytes == nullptr && "Malloc failed for keypress event creation!");
  // allBytes = fgets(allBytes, numBytes, stdin);
  for (int i = 0; i < numBytes; i++) {
    allBytes[i] = inputBytes[i];
  }
  allBytes[numBytes] = 0;
  // graphics_printf("Bytes (%d) read: %c\n",numBytes,*allBytes);
  // specialKey = NONE;
  if (*allBytes == '\e') {
    processSpecialBytes();
  }
  c = allBytes[numBytes-1];
}

void KeyPressEvent::processSpecialBytes(void) {
  assert(*allBytes == '\e' && "Cannot process special bytes for non control sequence bytes!");
  if (numBytes == 1) { // just the '\e'
    specialKey = ESC;
    // graphics_printf("Got ESC!\n");
    return;
  }
  // assert(isupper(allBytes[numBytes-1]) && "Control sequence must end with capital letter!");
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
    alt = true;
    c = allBytes[1];
    specialKey = ALT_LOWERCASE;
  }
}

// KeyPressEvent::~KeyPressEvent() {
//   free(allBytes);
// }

static keyPressHandler *handler = 0;
void onKeyPress(keyPressHandler _handler) {
  handler = _handler;
}

static char c;
static bool quit;

void keyboard_preloop() {
  assert(handler != 0 && "Must set keypress handler if using keyboard main!");

  quit = false;
}


void keyboard_loop(void) {
  static int n = 0;
  while (ioctl(STDIN_FILENO, FIONREAD, &n) == 0 && n > 0) {
    // char *buffer = (char *) malloc(sizeof(char) * n);
    // assert (buffer == nullptr && "Malloc failed for keypress event creation!");
    assert(n <= MAX_INPUT_BYTES && "Cannot buffer more than MAX_INPUT_BYTES characters! must input slower");
    static char buffer[MAX_INPUT_BYTES];
    for (int i = 0; i < n; i++) {
      buffer[i] = fgetc(stdin);
      // graphics_printf("read byte: %c|%d\n", buffer[i], buffer[i]);
    }
    // bool inControlSequence = false;
    int controlSeqStart = -1;
    int controlSeqEnd = -1;

    for (int i = 0; i < n && !quit; i++) {
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
          quit = handler(KeyPressEvent(buffer + controlSeqStart, controlSeqEnd - controlSeqStart + 1));
          controlSeqStart = -1;
          controlSeqEnd = -1;
        }
      } else if (buffer[i] == '\e') {
        controlSeqStart = i;
        // graphics_printf("start control sequence! %d", i);
      } else if (controlSeqStart == -1) {
        quit = handler(KeyPressEvent(buffer[i]));
      }
    }
    if (controlSeqStart != -1) {
      controlSeqEnd = n-1;
      quit = handler(KeyPressEvent(buffer + controlSeqStart, controlSeqEnd - controlSeqStart + 1));
    }
    // free(buffer);
  }
}

void keyboard_finish(int signum) {

}

int keyboard_main(int argc, char *argv[]) {
  #ifdef USE_KEYBOARD
  
  signal(SIGINT, keyboard_finish);
  signal(SIGTERM, keyboard_finish);

  keyboard_preloop();
  graphics_preloop(argc, argv);
  
  while (!quit) {
    keyboard_loop();
    graphics_loop();
  }

  keyboard_finish();
  graphics_finish();
  return 0;
  #else
  return graphics_main(argc, argv);
  #endif
}