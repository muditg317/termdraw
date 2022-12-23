#pragma once

namespace termdraw {

template<class... Capabilities>
template<typename... Tuples>
Application<Capabilities...>::Application(Tuples&&... tuples) { // : capabilities(std::forward<Tuples>(tuples)...) {
  // this->capabilities = std::make_tuple(Capabilities()...);
  // for (auto &capability : this->capabilities) {
  //   capability.setup();
  // }
}


template<class Capability>
Capability& ApplicationBase::getCapability(void) {
  auto capability = this->capabilities.find(typeid(Capability).name());
  if (capability == this->capabilities.end()) {
    throw std::runtime_error("Capability not found!");
  }
  return *std::static_pointer_cast<Capability>(capability->second);
}


template<class Capability>
void ApplicationBase::addCapability(void) {
  this->capabilities.emplace(std::make_pair(
    typeid(Capability).name(),
    std::make_shared<Capability>()
  ));
}

} // namespace termdraw