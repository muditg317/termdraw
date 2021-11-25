#include <termdraw/graphics.hpp>
#include <termdraw/keyboard.hpp>

#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <csignal>

#define USE_KEYBOARD

#define NB_DISABLE 0
#define NB_ENABLE 1

static struct timeval tv;
static fd_set fds;

inline int inputAvailable() {
  // tv.tv_sec = 0;
  // tv.tv_usec = 0;
  // FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
  select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &fds);
}

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
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0

  quit = false;
}

void keyboard_loop(void) {
  if (inputAvailable()) {
    c = fgetc(stdin);
    // graphics_printf("\b");
    quit = handler(c);
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