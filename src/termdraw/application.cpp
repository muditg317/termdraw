#include <termdraw/application.hpp>

#include <cstdio>
#include <csignal>

namespace termdraw {

static bool appCreated = false;
ApplicationBase::ApplicationBase(void)
    : quit_application(false) {
  if (appCreated) {
    throw std::runtime_error("Application already exists!");
  }
  appCreated = true;
}

void ApplicationBase::registerCapsAndDeps(std::vector<std::shared_ptr<CapabilityBase>>& capPtrs) {
  std::vector<std::shared_ptr<CapabilityBase>> allCaps;
  while (capPtrs.size() > 0) {
    auto capability = capPtrs.back();
    capPtrs.pop_back();
    allCaps.push_back(capability);
    auto deps = capability->loadDependencies();
    for (auto& dep : deps) {
      if (std::find(allCaps.begin(), allCaps.end(), dep) == allCaps.end()) {
        capPtrs.push_back(dep);
      }
    }
  }
  for (auto it = allCaps.rbegin(); it != allCaps.rend(); ++it) {
    registerCapability(*it);
  }
}


void ApplicationBase::registerPreloop(std::function<CapabilityBase::preloopFunc> func) {
  this->preloopFunctionRegistry.registerFunction(func);
}

void ApplicationBase::registerLoop(std::function<CapabilityBase::loopFunc> func) {
  this->loopFunctionRegistry.registerFunction(func);
}

void ApplicationBase::registerFinish(std::function<CapabilityBase::finishFunc> func) {
  this->finishFunctionRegistry.registerFunction(func);
}

void ApplicationBase::preloop(int argc, char *argv[]) {
  this->preloopFunctionRegistry(argc, argv);
}

void ApplicationBase::loop(void) {
  this->loopFunctionRegistry();
}

void ApplicationBase::finish(int signum) {
  this->finishFunctionRegistry(signum);
  if (signum) {
    exit(signum);
  }
}

void ApplicationBase::configure(void) {
  this->lockRegistry();
  this->setupSignalHandlers();
}

void ApplicationBase::lockRegistry(void) {
  this->preloopFunctionRegistry.lock();
  this->loopFunctionRegistry.lock();
  this->finishFunctionRegistry.lock();
}

void ApplicationBase::setupSignalHandlers(void) {
  struct sigaction signal_handler;
  signal_handler.sa_handler = ApplicationBase::staticSignalHandler;
  signal_handler.sa_flags = 0;
  sigemptyset(&signal_handler.sa_mask);

  sigaction(SIGINT, &signal_handler, nullptr);
  sigaction(SIGTERM, &signal_handler, nullptr);
  sigaction(SIGUSR1, &signal_handler, nullptr);
}

void ApplicationBase::staticSignalHandler(int signum) {
  app->finish(signum);
}

int ApplicationBase::run(int argc, char *argv[]) {
  this->configure();

  preloop(argc, argv);

  while (!quit_application) {
    loop();
  }

  finish();
  return 0;
}

void ApplicationBase::quit(void) {
  this->quit_application = true;
  raise(SIGUSR1);
}


} // namespace termdraw

int main(int argc, char *argv[]) {
  if (!termdraw::app) {
    printf("APPLICATION NOT SETUP!\n");
  }
  return termdraw::app->run(argc, argv);
}