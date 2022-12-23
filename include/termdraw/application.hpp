#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <termdraw/base.hpp>
#include <termdraw/capability.hpp>
#include <termdraw/function-registry.hpp>

#include <map>
#include <memory>
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>

#define DEFINE_APPLICATION(APP) \
  std::shared_ptr<termdraw::ApplicationBase> termdraw::app = std::make_shared<APP>();


namespace termdraw {

class ApplicationBase {

// application flow
 public:
  ApplicationBase();
  int run(int argc, char *argv[]);
 protected:
  void quit(void);
 private:
  bool quit_application;

// capabilities
 public:
  template<class Capability>
  Capability& getCapability(void);
 protected:
  template<class Capability>
  void addCapability(void);
 private:
  std::map<std::string, std::shared_ptr<Capability>> capabilities;

// inner workings
 protected:
  void preloop(int argc, char *argv[]);
  void loop(void);
  void finish(int signum = 0);

 private:
  void configure(void);
  void setupSignalHandlers(void);
  static void staticSignalHandler(int signum);

// function registry
 protected:
  void registerPreloop(std::function<preloopFunc>);
  void registerLoop(std::function<loopFunc>);
  void registerFinish(std::function<finishFunc>);
 private:
  void lockRegistry(void);
  FunctionRegistry<preloopFunc> preloopFunctionRegistry;
  FunctionRegistry<loopFunc> loopFunctionRegistry;
  FunctionRegistry<finishFunc> finishFunctionRegistry;
};

template<class... Capabilities>
class Application : public ApplicationBase {
 static_assert(std::conjunction_v<std::is_base_of<Capability, Capabilities>...>,
               "All template parameters must be derived from Capability");
 public:
 // take in tuple for args to each capability
  template<typename... Tuples>
  Application(Tuples&&... tuples);
};

extern std::shared_ptr<ApplicationBase> app;
} // namespace termdraw

#include <termdraw/application.ipp>

#endif