add_executable(app app.cpp)
# target_compile_features(app PRIVATE cxx_std_17)

target_link_libraries(app PRIVATE linalg_library fmt::fmt)

# target_link_options(app PRIVATE "-v")