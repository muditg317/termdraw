# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
set(C-Ballz_INCLUDE_DIR "${C-Ballz_SOURCE_DIR}/include")
file(GLOB LINALG_HEADER_LIST CONFIGURE_DEPENDS "${C-Ballz_INCLUDE_DIR}/linalg/*.hpp")

# Make an automatic library - will be static or dynamic based on user setting
add_library(linalg_library vectors.cpp ${LINALG_HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(linalg_library PUBLIC ${C-Ballz_INCLUDE_DIR})

# This depends on (header only) boost
# target_link_libraries(linalg_library PRIVATE Boost::boost)

# All users of this library will need at least C++11
# target_compile_features(linalg_library PUBLIC cxx_std_11)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${LINALG_HEADER_LIST})
