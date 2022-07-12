#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <functional>
#include <memory>
#include <type_traits>
#include <vector>

template<typename ReturnType, typename... ArgTypes>
class FunctionRegistry;

template<typename ReturnType, typename... ArgTypes>
class FunctionRegistry<ReturnType(ArgTypes...)> {
 public:
  typedef std::function<ReturnType(ArgTypes...)> __func_type;

  FunctionRegistry(void) : locked(false) {}

  void registerFunction(__func_type f);

  void lock(void);

  std::vector<ReturnType> callFunctions(ArgTypes... args);

 private:
  std::vector<__func_type> functions;
  bool locked;
};

class Application {

// general
 public:
  Application();
  int run(int argc, char *argv[]);
 protected:
  void quit(void);
 private:
  void preloop(int argc, char *argv[]);
  void loop(void);
  void finish(int signum = 0);

  void configure(void);
  void setupSignalHandlers(void);
  bool quit_application;
  static void staticSignalHandler(int signum);


// function registry
 protected:
  typedef void preloopFunc(int argc, char *argv[]);
  typedef void loopFunc(void);
  typedef void finishFunc(int signum);

  void registerPreloop(preloopFunc);
  void registerLoop(loopFunc);
  void registerFinish(finishFunc);
 private:
  void lockRegistry(void);
  FunctionRegistry<preloopFunc> preloopFunctionRegistry;
  FunctionRegistry<loopFunc> loopFunctionRegistry;
  FunctionRegistry<finishFunc> finishFunctionRegistry;
};

extern std::shared_ptr<Application> app;

#endif