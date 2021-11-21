#include <linalg/vectors.hpp>

#include <fmt/format.h>

#include <iostream>
#include <vector>
#include <tuple>

int main() {
    printf("Hello world!\n");

    auto vec = as_vec<int, 5>(5);
    vec_print(vec, "test");

    return 0;
}
