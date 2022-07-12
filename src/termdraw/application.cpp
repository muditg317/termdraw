#include <application.hpp>

#include <cstdio>
#include <csignal>


void Application::preloop(int argc, char *argv[]) {
  // graphics_printf("run preloops: %d\n", numPreloopFuncs);
  for (int8_t i = numPreloopFuncs-1; i >= 0; --i) {
    // graphics_printf("run preloop %d\n", i);
    (this->*preloopFuncs[i])(argc, argv);
  }
}

void Application::loop(void) {
  for (int8_t i = numLoopFuncs-1; i >= 0; --i) {
    (this->*loopFuncs[i])();
  }
}

void Application::finish(int signum = 0) {
  for (int8_t i = numFinishFuncs-1; i >= 0; --i) {
    (this->*finishFuncs[i])(signum);
  }
  if (signum) {
    exit(signum);
  }
}

void Application::setupSignalHandlers(void) {
  struct sigaction signal_handler = {
    .__sigaction_handler = this->finish,
    .sa_flags = 0
  };
  sigemptyset(&signal_handler.sa_mask);

  sigaction(SIGINT, &signal_handler, nullptr);
  sigaction(SIGTERM, &signal_handler, nullptr);
}

int Application::run(int argc, char *argv[]) {
  this->setupSignalHandlers();

  preloop(argc, argv);

  while (!quit_application) {
    loop();
  }

  finish();
  return 0;
}

int main(int argc, char *argv[]) {
  if (!app) {
    printf("APPLICATION NOT SETUP!\n");
  }
  return app->run(agrc, argv);
}