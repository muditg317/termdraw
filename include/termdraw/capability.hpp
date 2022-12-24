#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Eigen/Eigen>

#include <termdraw/base.hpp>

namespace termdraw {

class ApplicationBase;

/**
 * @brief The CapabilityBase class
 * This class is used to represent/add a capability of an application.
 */
class CapabilityBase : public std::enable_shared_from_this<CapabilityBase> {
 public:
  CapabilityBase();
  
  CapabilityBase(CapabilityBase const&) = delete;
  CapabilityBase& operator=(CapabilityBase const&) = delete;

  std::string getName() const;
  std::string getDescription() const;

 private:
  virtual std::vector<std::shared_ptr<CapabilityBase>> loadDependencies(void) = 0;

  virtual preloopFunc preloop = 0;
  virtual loopFunc loop = 0;
  virtual finishFunc finish = 0;

  friend class ApplicationBase;
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


} // namespace termdraw


#include <termdraw/capability.ipp>