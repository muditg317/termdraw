#pragma once

#include <memory>
#include <vector>

namespace termdraw {

template<class Capability>
Capability& ApplicationBase::getCapability(void) {
  auto capability = this->capabilities.find(typeid(Capability).name());
  if (capability == this->capabilities.end()) {
    throw std::runtime_error("Capability not found!");
  }
  return *std::static_pointer_cast<Capability>(capability->second);
}

template<class Capability>
void ApplicationBase::registerCapability(std::shared_ptr<Capability> capability) {
  this->capabilities.emplace(std::make_pair(
    typeid(Capability).name(),
    capability
  ));
  registerPreloop(std::bind(&Capability::preloop, capability.get(), std::placeholders::_1, std::placeholders::_2));
  registerLoop(std::bind(&Capability::loop, capability.get()));
  registerFinish(std::bind(&Capability::finish, capability.get(), std::placeholders::_1));
}


template<class Capability,
         class... Args,
         std::enable_if_t<std::is_constructible<Capability, Args&&...>::value, int> = 0>
inline void buildCapability(std::vector<std::shared_ptr<CapabilityBase>>& capsList, std::tuple<Args...> args) {
  capsList.push_back(std::make_shared<Capability>(std::forward<Args>(std::get<Args>(args))...));
}

template<class... Capabilities>
template<typename... Tuples>
Application<Capabilities...>::Application(Tuples&&... tuples) {
  std::vector<std::shared_ptr<CapabilityBase>> capPtrs;
  if constexpr (sizeof...(Tuples) > 0) {
    (buildCapability<Capabilities>(capPtrs, std::forward<Tuples>(tuples)), ...);
  }
  this->registerCapsAndDeps(capPtrs);
}

} // namespace termdraw