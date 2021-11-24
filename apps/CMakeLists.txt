
find_package(box2d REQUIRED)
message(STATUS "box2d::box2d version: ${box2d_VERSION}")# Adds Boost::boost

add_executable(app app.cpp)
target_compile_features(app PRIVATE cxx_std_17)

target_link_libraries(app PRIVATE termdraw_library fmt::fmt box2d::box2d)
# target_link_options(app PRIVATE "-v")