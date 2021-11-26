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

static struct timeval tv;
static fd_set fds;

inline int inputAvailable() {
  FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
  select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &fds);


  // struct pollfd pollfd;
  // int ret;
  // pollfd.fd = STDIN_FILENO; /* this is STDIN */
  // pollfd.events = POLLIN;
  // ret = poll(&pollfd, 1, 0);
  // return ret;

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

  quit = false;
}

void keyboard_loop(void) {
  
  int n = 0;
  int ioRes = ioctl(STDIN_FILENO, FIONREAD, &n);
  while (ioRes == 0 && n > 0) {
    std::vector<char> bytes(n);

    while (n--) {
      bytes.push_back(fgetc(stdin));
      graphics_printf("read byte: %c|%d",bytes.back(),bytes.back());
      c = bytes.back();
    }

    // c = fgetc(stdin);
    // if (c == '\e') { // got '\033'
    //   // graphics_printf("special character pressed! %c|%d\n",c,c);
    //   // graphics_printf("more available: %d\n",inputAvailable());
    //   graphics_printf("got special %c|%d\n",c,c);
    //   c = fgetc(stdin);
    //   graphics_printf("got special %c|%d\n",c,c);
    //   c = fgetc(stdin);
    //   graphics_printf("got special %c|%d\n",c,c);
    // }

    // int n = 0;
    // int ioRes = ioctl(STDIN_FILENO, FIONREAD, &n);
    // graphics_printf("ioctl result: %d, n:%d\n", ioRes, n);
    // while (ioRes == 0 && n > 0) {
    //   graphics_printf("bytes available: %d\n",n);
    //   c = fgetc(stdin);
    //   graphics_printf("reading |c:%c|d:%d|\n",c,c);
    //   // if (c=='\e') {
    //   //   graphics_printf("got \\e! waiting...\n");
    //   //   usleep(1000000);
    //   //   graphics_printf("done waiting!\n");
    //   // }
    //   ioRes = ioctl(STDIN_FILENO, FIONREAD, &n);
    //   graphics_printf("ioctl result: %d, n:%d\n", ioRes, n);
    // }

    // char str[100];
    // // scanf("%s",str);
    // int i = 0;
    // while ((*(str+i++) = fgetc(stdin)) != EOF);
    // str[i]=0;
    // // fgets(str, 10, stdin);
    // graphics_printf("read string: %s\n",str);
    quit = handler(c);
    ioRes = ioctl(STDIN_FILENO, FIONREAD, &n);
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