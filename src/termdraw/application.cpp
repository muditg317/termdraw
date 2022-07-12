#include <termdraw/application.hpp>

#include <cstdio>
#include <csignal>

template<typename ReturnType, typename... ArgTypes>
void FunctionRegistry<ReturnType(ArgTypes...)>::registerFunction(__func_type func) {
  if (locked) {
    throw std::runtime_error("Function registry is locked! Cannot register new function");
  }
  this->functions.push_back(func);
}

template<typename ReturnType, typename... ArgTypes>
void FunctionRegistry<ReturnType(ArgTypes...)>::lock(void) {
  this->locked = true;
}

template<typename ReturnType, typename... ArgTypes>
std::vector<ReturnType> FunctionRegistry<ReturnType(ArgTypes...)>::callFunctions(ArgTypes... args) {
  if (!locked) {
    throw std::runtime_error("Function registry is not locked! Cannot call functions\n");
  }
  std::vector<ReturnType> results;
  for (auto func : this->functions) {
    results.push_back(func(args...));
  }
  return results;
}

Application::Application(void)
    : quit_application(false) {
  // this->registerPreloop(std::bind(&Application::preloop, this, std::placeholders::_1, std::placeholders::_2));
  // this->registerLoop(this->loop);
  // this->registerFinish(this->finish);
}

void Application::registerPreloop(preloopFunc func) {
  this->preloopFunctionRegistry.registerFunction(func);
}

void Application::registerLoop(loopFunc func) {
  this->loopFunctionRegistry.registerFunction(func);
}

void Application::registerFinish(finishFunc func) {
  this->finishFunctionRegistry.registerFunction(func);
}

void Application::preloop(int argc, char *argv[]) {
  this->preloopFunctionRegistry.callFunctions(argc, argv);
}

bool Application::loop(void) {
  this->loopFunctionRegistry.callFunctions();
}

void Application::finish(int signum = 0) {
  this->finishFunctionRegistry.callFunctions(signum);
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

int main(int argc, char *argv[]) {
  if (!app) {
    printf("APPLICATION NOT SETUP!\n");
  }
  return app->run(agrc, argv);
}