#include <termdraw/application.hpp>

#include <cstdio>
#include <csignal>

static bool appCreated = false;
Application::Application(void)
    : quit_application(false) {
  if (appCreated) {
    throw std::runtime_error("Application already exists!");
  }
  appCreated = true;
}

void Application::registerPreloop(std::function<preloopFunc> func) {
  this->preloopFunctionRegistry.registerFunction(func);
}

void Application::registerLoop(std::function<loopFunc> func) {
  this->loopFunctionRegistry.registerFunction(func);
}

void Application::registerFinish(std::function<finishFunc> func) {
  this->finishFunctionRegistry.registerFunction(func);
}

void Application::preloop(int argc, char *argv[]) {
  this->preloopFunctionRegistry(argc, argv);
}

void Application::loop(void) {
  this->loopFunctionRegistry();
}

void Application::finish(int signum) {
  this->finishFunctionRegistry(signum);
  if (signum) {
    exit(signum);
  }
}

void Application::configure(void) {
  this->lockRegistry();
  this->setupSignalHandlers();
}

void Application::lockRegistry(void) {
  this->preloopFunctionRegistry.lock();
  this->loopFunctionRegistry.lock();
  this->finishFunctionRegistry.lock();
}

void Application::setupSignalHandlers(void) {
  struct sigaction signal_handler = {
    .__sigaction_handler = Application::staticSignalHandler,
    .sa_flags = 0
  };
  sigemptyset(&signal_handler.sa_mask);

  sigaction(SIGINT, &signal_handler, nullptr);
  sigaction(SIGTERM, &signal_handler, nullptr);
  sigaction(SIGUSR1, &signal_handler, nullptr);
}

void Application::staticSignalHandler(int signum) {
  app->finish(signum);
}

int Application::run(int argc, char *argv[]) {
  this->configure();

  preloop(argc, argv);

  while (!quit_application) {
    loop();
  }

  finish();
  return 0;
}

void Application::quit(void) {
  this->quit_application = true;
  raise(SIGUSR1);
}

int main(int argc, char *argv[]) {
  if (!app) {
    printf("APPLICATION NOT SETUP!\n");
  }
  return app->run(argc, argv);
}