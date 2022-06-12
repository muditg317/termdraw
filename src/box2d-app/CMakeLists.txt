# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
file(GLOB_RECURSE BOX2D_APP_HEADER_LIST CONFIGURE_DEPENDS "${C-Ballz_INCLUDE_DIR}/box2d-app/*.hpp")
file(GLOB_RECURSE BOX2D_APP_SOURCE_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Make an automatic library - will be static or dynamic based on user setting
add_library(LibBox2dApp ${BOX2D_APP_SOURCE_LIST} ${BOX2D_APP_HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(LibBox2dApp PUBLIC ${C-Ballz_INCLUDE_DIR})

# depends on box2d library
find_package(box2d REQUIRED)
message(STATUS "box2d::box2d version: ${box2d_VERSION}")# Adds box2d::box2d

target_link_libraries(LibBox2dApp PUBLIC box2d::box2d termdraw_library)

# All users of this library will need at least C++17
target_compile_features(LibBox2dApp PUBLIC cxx_std_17)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${BOX2D_APP_HEADER_LIST})
