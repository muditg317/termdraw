#ifndef CBALLS_UTILS_HPP
#define CBALLS_UTILS_HPP

#include <Eigen/Eigen>

namespace Eigen {
  typedef Eigen::Matrix<std::complex<int>,Eigen::Dynamic,Eigen::Dynamic> MatrixXci;
}

template<typename R, typename... A>
R ret(R(*)(A...));

template<typename C, typename R, typename... A>
R ret(R(C::*)(A...));



#endif