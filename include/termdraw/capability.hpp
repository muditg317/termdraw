#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Eigen/Eigen>

#include <termdraw/base.hpp>

namespace termdraw {

class ApplicationBase;

template<typename>
struct PM_traits {}; 

template<class T, class U>
struct PM_traits<U T::*> {
  using member_type = U;
};

/**
 * @brief The CapabilityBase class
 * This class is used to represent/add a capability of an application.
 */
class CapabilityBase : public std::enable_shared_from_this<CapabilityBase> {
 public:
  CapabilityBase();

  CapabilityBase(CapabilityBase const&) = delete;
  CapabilityBase& operator=(CapabilityBase const&) = delete;

  virtual std::string name() const = 0;
  // std::string getDescription() const;

 private:
  virtual std::vector<std::shared_ptr<CapabilityBase>> loadDependencies(void) = 0;

  virtual void preloop(int argc, char *argv[]) = 0;
  virtual void loop(void) = 0;
  virtual void finish(int signum) = 0;

  friend class ApplicationBase;

 public:
  typedef PM_traits<decltype(&CapabilityBase::preloop)>::member_type preloopFunc;
  typedef PM_traits<decltype(&CapabilityBase::loop)>::member_type loopFunc;
  typedef PM_traits<decltype(&CapabilityBase::finish)>::member_type finishFunc;
  // typedef decltype(&CapabilityBase::preloop) preloopFunc;
  // typedef decltype(&CapabilityBase::loop) loopFunc;
  // typedef decltype(&CapabilityBase::finish) finishFunc;
};

template<class... Dependencies>
class Capability : public CapabilityBase {
 static_assert(std::conjunction_v<std::is_base_of<CapabilityBase, Dependencies>...>,
               "All Capability template parameters must be derived from CapabilityBase");

  typedef std::tuple<std::shared_ptr<Dependencies>...> dependencyTuple_t;

 public:
  template<typename... DependencyArgGroups>
  Capability(DependencyArgGroups&&...);

 private:
  template<class Dependency,
        class... Args,
        std::enable_if_t<std::is_constructible<Dependency, Args&&...>::value, int> = 0>
  void buildDependency(std::tuple<Args...> args);
  dependencyTuple_t dependencies;
  std::vector<std::shared_ptr<CapabilityBase>> loadDependencies(void) override;
};

template<class... Dependencies>
class DependentCapability : public Capability<Dependencies...> {
 public:
  template<typename... DependencyArgGroups>
  DependentCapability(DependencyArgGroups&&... dependencyArgs) : Capability<Dependencies...>(std::forward<DependencyArgGroups>(dependencyArgs)...) {};

 private:
  void preloop(int argc, char *argv[]) override {};
  void loop(void) override {};
  void finish(int signum) override {};
};

} // namespace termdraw


#include <termdraw/capability.ipp>