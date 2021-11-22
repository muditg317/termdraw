#ifndef VECTORS_HPP
#define VECTORS_HPP

#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <execution>


typedef std::vector<float> vecf;
typedef std::vector<int> veci;

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

template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b) {
  assert(a.size() == b.size());

  std::vector<T> result;
  result.reserve(a.size());

  std::transform(a.begin(), a.end(), b.begin(), 
                  std::back_inserter(result), std::plus<T>());
  return result;
}

template <typename T>
std::vector<T> operator-(const std::vector<T>& a, const std::vector<T>& b) {
  assert(a.size() == b.size());

  std::vector<T> result;
  result.reserve(a.size());

  std::transform(a.begin(), a.end(), b.begin(), 
                  std::back_inserter(result), std::minus<T>());
  return result;
}

template <typename T>
std::vector<T> operator*(const std::vector<T>& a, const std::vector<T>& b) {
  assert(a.size() == b.size());

  std::vector<T> result;
  result.reserve(a.size());

  std::transform(a.begin(), a.end(), b.begin(), 
                  std::back_inserter(result), std::multiplies<T>());
  return result;
}

template <typename T>
std::vector<T> operator/(const std::vector<T>& a, const std::vector<T>& b) {
  assert(a.size() == b.size());

  std::vector<T> result;
  result.reserve(a.size());

  std::transform(a.begin(), a.end(), b.begin(), 
                  std::back_inserter(result), std::divides<T>());
  return result;
}

template <typename T>
std::vector<T> vec_floor(const std::vector<T>& a) {
  std::vector<T> result;
  result.reserve(a.size());

  std::for_each(std::execution::par_unseq,
            a.begin(),
            a.end(),
            [](T& val){ result.pushback(std::floor(val)); });

  return result;
}

template <typename T>
veci vec_2int(const std::vector<T>& a) {
    std::vector<T> result;
    result.reserve(a.size());

    std::for_each(std::execution::par_unseq,
              a.begin(),
              a.end(),
              [](T& val){ result.pushback((int) val)); });

    return result;
}

#endif