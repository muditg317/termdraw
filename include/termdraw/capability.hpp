#ifndef CAPABILITY_HPP
#define CAPABILITY_HPP

#include <string>
#include <vector>

#include <Eigen/Eigen>

#include <termdraw/base.hpp>

namespace termdraw {


/**
 * @brief The Capability class
 * This class is used to represent/add a capability of an application.
 */
class Capability {
 public:
  Capability();
  
  Capability(Capability const&) = delete;
  Capability& operator=(Capability const&) = delete;


  std::string getName() const;
  std::string getDescription() const;

 private:
  preloopFunc preloopFunction;
  loopFunc loopFunction;
  finishFunc finishFunction;
};

} // namespace termdraw
#endif // CAPABILITY_HPP
