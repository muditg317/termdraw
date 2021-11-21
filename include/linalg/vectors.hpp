#ifndef VECTORS_HPP
#define VECTORS_HPP

#include <vector>
#include <tuple>
#include <iostream>

template<typename T, int N>
std::vector<T> as_vec(T val) {
  return std::vector<T>(N, val);
}

template<typename T>
void vec_print(std::vector<T> to_print, std::string label) {
  printf("Vector `%s`:\t<", label.c_str());
  for (auto val : to_print) {
    std::cout << val << ',';
  }
  std::cout << "\b>" << std::endl;
}

template<typename T>
std::vector<T> vec_sum(std::vector<T>...);

template<typename T>
std::vector<T> vec_prod(std::vector<T>...);

#endif