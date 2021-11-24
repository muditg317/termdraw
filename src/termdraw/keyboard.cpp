#include <termdraw/graphics.hpp>
#include <termdraw/keyboard.hpp>

#include <sys/select.h>
#include <termios.h>
#include <unistd.h>


#define NB_DISABLE 0
#define NB_ENABLE 1


inline int inputAvailable() {
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
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

keyPressHandler *handler = 0;
void onKeyPress(keyPressHandler _handler) {
  handler = _handler;
}

int keyboard_main(int argc, char *argv[]) {

  auto t = preloop(argc, argv);

  assert(handler != 0 && "Must set keypress handler if using keyboard main!");

  char c;
  bool i = false;
  setNonBlocking(NB_ENABLE);
  while (!i) {
    if (inputAvailable()) {
      c=fgetc(stdin);
      // printf("\x1b[1D");
      printf("\b");
      // if (handler) {
      // dont need to check for handler existence because of assertion
      i = handler(c);
      // }
    }
    loop(t);
  }
  setNonBlocking(NB_DISABLE);

  finish();
  return 0;
}