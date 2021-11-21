#pragma once

#include <vector>
#include <tuple>

template<typename T, int N>
std::vector<T> as_vec(T);

template<typename T>
std::vector<T> vec_sum(std::vector<T>...);

template<typename T>
std::vector<T> vec_prod(std::vector<T>...);

