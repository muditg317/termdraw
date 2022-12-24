#pragma once

#include <string>
#include <vector>

#include <Eigen/Eigen>

#include <termdraw/base.hpp>

namespace termdraw {

template<class... Dependencies>
template<class Dependency,
         class... Args,
         std::enable_if_t<std::is_constructible<Dependency, Args&&...>::value, int>>
void Capability<Dependencies...>::buildDependency(std::tuple<Args...> args) {
  std::get<std::shared_ptr<Dependency>>(this->dependencies) = std::make_shared<Dependency>(std::forward<Args>(std::get<Args>(args))...);
}

template<class... Dependencies>
template<typename... DependencyArgGroups>
Capability<Dependencies...>::Capability(DependencyArgGroups&&... dependencyArgs) {
  if constexpr (sizeof...(DependencyArgGroups) > 0) {
    (buildDependency<Dependencies>(std::forward<DependencyArgGroups>(dependencyArgs)), ...);
  }
  // app.registerCapability<Capability<Dependencies...>>(this);
}

template<class... Dependencies>
std::vector<std::shared_ptr<CapabilityBase>> Capability<Dependencies...>::loadDependencies(void) {
  return std::vector<std::shared_ptr<CapabilityBase>>{std::static_pointer_cast<CapabilityBase>(std::get<std::shared_ptr<Dependencies>>(dependencies))...};
}


} // namespace termdraw
