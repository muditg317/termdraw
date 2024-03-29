# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
file(GLOB_RECURSE TERMDRAW_HEADER_LIST CONFIGURE_DEPENDS "${C-Ballz_INCLUDE_DIR}/termdraw/*.hpp")
file(GLOB_RECURSE TERMDRAW_SOURCE_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Make an automatic library - will be static or dynamic based on user setting
add_library(termdraw_library ${TERMDRAW_SOURCE_LIST} ${TERMDRAW_HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(termdraw_library PUBLIC ${C-Ballz_INCLUDE_DIR})

# This depends on (header only) Eigen3 library
find_package(Eigen3 CONFIG REQUIRED)
message(STATUS "Eigen3::Eigen version: ${Eigen3_VERSION}")# Adds Eigen3::Eigen

target_link_libraries(termdraw_library PRIVATE Eigen3::Eigen fmt::fmt)

# All users of this library will need at least C++17
target_compile_features(termdraw_library PUBLIC cxx_std_17)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${TERMDRAW_HEADER_LIST})
