#include <termdraw/graphics.hpp>
#include <termdraw/keyboard.hpp>

#include <iostream>
#include <sys/select.h>
#include <sys/poll.h>
#include <termios.h>
#include <unistd.h>
#include <csignal>
#include <sys/ioctl.h>
// #include <sys/str
// #include

#define USE_KEYBOARD

#define NB_DISABLE 0
#define NB_ENABLE 1



KeyPressEvent::KeyPressEvent(int numBytesToRead) : numBytes(numBytesToRead) {
  // graphics_printf("read %d bytes from buffer!", numBytes);
  allBytes = (char *) malloc(sizeof(char) * numBytes);
  assert (allBytes == nullptr && "Malloc failed for keypress event creation!");
  // allBytes = fgets(allBytes, numBytes, stdin);
  for (int i = 0; i < numBytes; i++) {
    allBytes[i] = fgetc(stdin);
    // graphics_printf("Byte read: %c|%d\n",allBytes[i],allBytes[i]);
  }
  specialKey = NONE;
  if (*allBytes == '\e') {
    processSpecialBytes();
  }
  c = allBytes[numBytes-1];
}

void KeyPressEvent::processSpecialBytes(void) {
  assert(*allBytes == '\e' && "Cannot process special bytes for non control sequence bytes!");
  if (numBytes == 1) { // just the '\e'
    specialKey = ESC;
    graphics_printf("Got ESC!\n");
    return;
  }

}

KeyPressEvent::~KeyPressEvent() {
  free(allBytes);
}

// static struct timeval tv;
// static fd_set fds;

// inline int inputAvailable() {
//   FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
//   select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
//   return FD_ISSET(STDIN_FILENO, &fds);


//   // struct pollfd pollfd;
//   // int ret;
//   // pollfd.fd = STDIN_FILENO; /* this is STDIN */
//   // pollfd.events = POLLIN;
//   // ret = poll(&pollfd, 1, 0);
//   // return ret;

// }

void setNonBlocking(bool state) {
  struct termios ttystate;
  //get the terminal state
  tcgetattr(STDIN_FILENO, &ttystate);

  if (state==NB_ENABLE) {
    //turn off canonical mode
    ttystate.c_lflag &= ~ICANON;
    //minimum of number input read.
    ttystate.c_cc[VMIN] = 1;
  } else if (state==NB_DISABLE) {
    //turn on canonical mode
    ttystate.c_lflag |= ICANON;
  }
  //set the terminal attributes.
  tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

static keyPressHandler *handler = 0;
void onKeyPress(keyPressHandler _handler) {
  handler = _handler;
}

static char c;
static bool quit;

void keyboard_preloop() {
  assert(handler != 0 && "Must set keypress handler if using keyboard main!");
  setNonBlocking(NB_ENABLE);
  // tv.tv_sec = 0;
  // tv.tv_usec = 0;
  // FD_ZERO(&fds);

  quit = false;
}


void keyboard_loop(void) {
  static int n = 0;
  while (ioctl(STDIN_FILENO, FIONREAD, &n) == 0 && n > 0) {
    // std::vector<char> bytes(n);
    // while (n--) {
    //   bytes.push_back(fgetc(stdin));
    //   graphics_printf("read byte: %c|%d",bytes.back(),bytes.back());
    //   // c = bytes.back();
    // }
    quit = handler(KeyPressEvent(n));
  }
}

void keyboard_finish(int signum) {
  setNonBlocking(NB_DISABLE);
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