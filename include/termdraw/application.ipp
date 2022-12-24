#pragma once

#include <iostream>
#include <memory>
#include <vector>

namespace termdraw {

template<class Capability>
Capability& ApplicationBase::getCapability(void) {
  auto capability = this->capabilities.find(Capability::NAME);
  if (capability == this->capabilities.end()) {
    throw std::runtime_error("Capability not found!");
  }
  return *std::static_pointer_cast<Capability>(capability->second);
}

template<class Capability>
void ApplicationBase::registerCapability(std::shared_ptr<Capability> capability) {
  this->capabilities.emplace(std::make_pair(
    capability->name(),
    capability
  ));
  // std::cout << "Registered capability: " << capability->name() << std::endl;
  
  // if Capability has preloop member function, register it
  if constexpr (std::is_member_function_pointer<decltype(&Capability::preloop)>::value) {
    registerPreloop(std::bind(&Capability::preloop, capability.get(), std::placeholders::_1, std::placeholders::_2));
  }
  // if Capability has loop member function, register it
  if constexpr (std::is_member_function_pointer<decltype(&Capability::loop)>::value) {
    registerLoop(std::bind(&Capability::loop, capability.get()));
  }
  // if Capability has finish member function, register it
  if constexpr (std::is_member_function_pointer<decltype(&Capability::finish)>::value) {
    registerFinish(std::bind(&Capability::finish, capability.get(), std::placeholders::_1));
  }

  // registerPreloop(std::bind(&Capability::preloop, capability.get(), std::placeholders::_1, std::placeholders::_2));
  // registerLoop(std::bind(&Capability::loop, capability.get()));
  // registerFinish(std::bind(&Capability::finish, capability.get(), std::placeholders::_1));
}


template<class Capability,
         class... Args,
         std::enable_if_t<std::is_constructible<Capability, Args&&...>::value, int> = 0>
inline void buildCapability(std::vector<std::shared_ptr<CapabilityBase>>& capsList, std::tuple<Args...> args) {
  capsList.push_back(std::make_shared<Capability>(std::forward<Args>(std::get<Args>(args))...));
}

template<class Capability>
inline void buildCapability(std::vector<std::shared_ptr<CapabilityBase>>& capsList) {
  capsList.push_back(std::make_shared<Capability>());
}

template<class... Capabilities>
template<typename... Tuples>
Application<Capabilities...>::Application(Tuples&&... tuples) {
  std::vector<std::shared_ptr<CapabilityBase>> capPtrs;
  if constexpr (sizeof...(Tuples) == 0) {
    (buildCapability<Capabilities>(capPtrs), ...);
  } else {
    (buildCapability<Capabilities>(capPtrs, std::forward<Tuples>(tuples)), ...);
  }
  this->registerCapsAndDeps(capPtrs);
}

} // namespace termdraw