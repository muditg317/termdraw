#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <termdraw/function-registry.hpp>

#include <memory>


class Application {

// application flow
 public:
  Application();
  int run(int argc, char *argv[]);
 protected:
  void quit(void);
 private:
  bool quit_application;

// inner workings
 private:
  void preloop(int argc, char *argv[]);
  void loop(void);
  void finish(int signum = 0);

  void configure(void);
  void setupSignalHandlers(void);
  static void staticSignalHandler(int signum);

// function registry
 protected:
  typedef void preloopFunc(int argc, char *argv[]);
  typedef void loopFunc(void);
  typedef void finishFunc(int signum);

  void registerPreloop(std::function<preloopFunc>);
  void registerLoop(std::function<loopFunc>);
  void registerFinish(std::function<finishFunc>);
 private:
  void lockRegistry(void);
  FunctionRegistry<preloopFunc> preloopFunctionRegistry;
  FunctionRegistry<loopFunc> loopFunctionRegistry;
  FunctionRegistry<finishFunc> finishFunctionRegistry;
};

extern std::shared_ptr<Application> app;

#endif