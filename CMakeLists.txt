# Works with 3.11 and tested through 3.22
cmake_minimum_required(VERSION 3.11...3.22)

# Project name and a few useful settings. Other commands can pick up the results
project(
  C-Ballz
  VERSION 0.1
  DESCRIPTION "Console visualization of physics simulated bouncy balls"
  LANGUAGES CXX)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

  # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here

  # Let's ensure -std=c++xx instead of -std=g++xx
  set(CMAKE_CXX_EXTENSIONS OFF)

  # Let's nicely support folders in IDEs
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Testing only available if this is the main app
  # Note this needs to be done in the main CMakeLists
  # since it calls enable_testing, which must be in the
  # main CMakeLists.
  # set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
  # include(CTest)
  # enable_testing()
endif()

# Accumulator library
include(/home/muditg317/application-installs/vcpkg/scripts/buildsystems/vcpkg.cmake)
# This is header only, so could be replaced with git submodules or FetchContent
# Adds fmt::fmt
find_package(fmt CONFIG REQUIRED)
message(STATUS "fmt::fmt version: ${fmt_VERSION}")# Adds fmt::fmt
# Adds Boost::filesystem
find_package(Boost REQUIRED)
message(STATUS "Boost::boost version: ${Boost_VERSION}")# Adds Boost::boost

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(apps)

# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING)
   AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
