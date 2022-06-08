set (apps
  pong
  # snake
)
# set (MAIN_APP snake)
list(GET apps -1 MAIN_APP)


set (DEFAULT_APP_LIBRARIES termdraw_library)

set (pong_LIBRARIES termdraw_library fmt::fmt LibBox2dApp)
set (snake_LIBRARIES termdraw_library fmt::fmt box2d::box2d LibBox2dApp)

set (APPS_OUTPUT_DIR ${C-Ballz_SOURCE_DIR})
if (NOT EXISTS ${APPS_OUTPUT_DIR})
  file(MAKE_DIRECTORY ${APPS_OUTPUT_DIR})
endif ()

foreach (app_name ${apps})
  # glob source files for the app
  if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.11.0")
      file(GLOB_RECURSE ${app_name}_SRCS CONFIGURE_DEPENDS ${app_name}/*.cpp ${app_name}/*.cc)
  else()
      file(GLOB ${app_name}_SRCS ${app_name}/*.cpp ${app_name}/*.cc)
  endif()

  # add executable for the app
  add_executable(${app_name} ${${app_name}_SRCS})
  target_compile_features(${app_name} PRIVATE cxx_std_17)
  # link any relevant libraries if present
  if(DEFINED ${app_name}_LIBRARIES)
    target_link_libraries(${app_name} PRIVATE ${${app_name}_LIBRARIES})
  else()
    target_link_libraries(${app_name} PRIVATE ${DEFAULT_APP_LIBRARIES})
  endif()

  # copy built apps back into source tree
  add_custom_command(TARGET ${app_name}
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${app_name}> ${APPS_OUTPUT_DIR})
endforeach()

# add_executable(app ALIAS ${MAIN_APP})
add_custom_command(TARGET ${MAIN_APP}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${MAIN_APP}> ${CMAKE_CURRENT_BINARY_DIR}/app)
