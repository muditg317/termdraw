# set (apps
#   pong
# )
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children LIST_DIRECTORIES true RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(apps ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "apps: ${apps}")
list(GET apps -1 MAIN_APP)


set (DEFAULT_APP_LIBRARIES termdraw_library)

set (pong_LIBRARIES termdraw_library fmt::fmt LibBox2dApp)
set (snake_LIBRARIES termdraw_library fmt::fmt LibBox2dApp)

set (APPS_OUTPUT_DIR ${C-Ballz_SOURCE_DIR})
if (NOT EXISTS ${APPS_OUTPUT_DIR})
  file(MAKE_DIRECTORY ${APPS_OUTPUT_DIR})
endif ()

foreach (app_name ${apps})
  # glob source files for the app
  if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.11.0")
      file(GLOB_RECURSE ${app_name}_SRCS CONFIGURE_DEPENDS ${app_name}/*.cpp ${app_name}/*.cc)
      file(GLOB_RECURSE ${app_name}_HDRS CONFIGURE_DEPENDS ${app_name}/*.hpp ${app_name}/*.h)
  else()
      file(GLOB ${app_name}_SRCS ${app_name}/*.cpp ${app_name}/*.cc)
      file(GLOB ${app_name}_HDRS ${app_name}/*.hpp ${app_name}/*.h)
  endif()

  # add executable for the app
  add_executable(${app_name} ${${app_name}_SRCS} ${${app_name}_HDRS})
  target_compile_features(${app_name} PRIVATE cxx_std_17)
  target_include_directories(${app_name} PUBLIC ${app_name})
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
