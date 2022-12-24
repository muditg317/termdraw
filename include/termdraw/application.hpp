#pragma once

#include <termdraw/base.hpp>
#include <termdraw/capability.hpp>
#include <termdraw/function-registry.hpp>

#include <map>
#include <memory>
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>

#define APP (termdraw::app)

#define DEFINE_APPLICATION(...) \
  std::shared_ptr<termdraw::ApplicationBase> termdraw::app = std::make_shared<termdraw::Application<__VA_ARGS__>>();

namespace termdraw {

class ApplicationBase {

// application flow
 public:
  ApplicationBase();
  int run(int argc, char *argv[]);
//  protected:
  void quit(void);
 private:
  bool quit_application;

// capabilities
 public:
  template<class Capability>
  Capability& getCapability(void);
 protected:
  void registerCapsAndDeps(std::vector<std::shared_ptr<CapabilityBase>>& capPtrs);
 private:
  template<class Capability>
  void registerCapability(std::shared_ptr<Capability>);

  std::map<std::string, std::shared_ptr<CapabilityBase>> capabilities;

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
  void registerPreloop(std::function<CapabilityBase::preloopFunc>);
  void registerLoop(std::function<CapabilityBase::loopFunc>);
  void registerFinish(std::function<CapabilityBase::finishFunc>);
 private:
  void lockRegistry(void);
  FunctionRegistry<CapabilityBase::preloopFunc> preloopFunctionRegistry;
  FunctionRegistry<CapabilityBase::loopFunc> loopFunctionRegistry;
  FunctionRegistry<CapabilityBase::finishFunc> finishFunctionRegistry;
};

template<class... Capabilities>
class Application : public ApplicationBase {
 static_assert(std::conjunction_v<std::is_base_of<CapabilityBase, Capabilities>...>,
               "All Application template parameters must be derived from CapabilityBase");
 public:
 // take in tuple for args to each capability
  template<typename... Tuples>
  Application(Tuples&&... tuples);

};

extern std::shared_ptr<ApplicationBase> app;
} // namespace termdraw

#include <termdraw/application.ipp>
