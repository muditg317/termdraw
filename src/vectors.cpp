#include <linalg/vectors.hpp>

#include <vector>
#include <cstdarg>

template<typename T, int N>
std::vector<T> as_vec(T val) {
  return std::vector<T>(N, val);
}

template<typename T>
std::vector<T> vec_sum(std::vector<T> to_add...) {
  // va_list args;
  // va_start(args, to_add);
  // std::plus
  // while (*to_add != '\0') {
  //     if (*fmt == 'd') {
  //         int i = va_arg(args, int);
  //         std::cout << i << '\n';
  //     } else if (*fmt == 'c') {
  //         // note automatic conversion to integral type
  //         int c = va_arg(args, int);
  //         std::cout << static_cast<char>(c) << '\n';
  //     } else if (*fmt == 'f') {
  //         double d = va_arg(args, double);
  //         std::cout << d << '\n';
  //     }
  //     ++fmt;
  // }

  // va_end(args);
}

template<typename T>
std::vector<T> vec_prod(std::vector<T>...);
