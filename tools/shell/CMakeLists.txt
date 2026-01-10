include_directories(include)
add_subdirectory(linenoise)
add_definitions(-DHAVE_LINENOISE=1)
include_directories(linenoise/include)
add_definitions(-DSQLITE_SHELL_IS_UTF8)
add_definitions(-DUSE_GOOSE_SHELL_WRAPPER)


if(GOOSE_EXTENSION_AUTOCOMPLETE_SHOULD_LINK)
  include_directories(../../extension/autocomplete/include)
  set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES}
                       ../../extension/autocomplete/autocomplete_extension.cc)
  add_definitions(-DSHELL_INLINE_AUTOCOMPLETE)
endif()
set(SHELL_SOURCES
    ${SHELL_SOURCES}
    shell_command_line_option.cc
    shell_extension.cc
    shell.cc
    shell_helpers.cc
    shell_metadata_command.cc
    shell_prompt.cc
    shell_renderer.cc
    shell_highlight.cc
    shell_progress_bar.cc
    shell_render_table_metadata.cc
    shell_windows.cc)

add_executable(shell ${SHELL_SOURCES})

target_link_libraries(shell goose_static ${GOOSE_EXTRA_LINK_FLAGS})
link_threads(shell "")
if(STATIC_LIBCPP)
  message("Statically linking CLI")
  target_link_libraries(shell -static-libstdc++ -static-libgcc)
endif()

link_extension_libraries(shell "")

function(ensure_variable_is_number INPUT_VERSION OUT_RESULT)
  if(NOT "${${INPUT_VERSION}}" MATCHES "^[0-9]+$")
    message(
      WARNING
        "VERSION PARAMETER ${INPUT_VERSION} \"${${INPUT_VERSION}}\" IS NOT A NUMBER - SETTING TO 0"
    )
    set(${OUT_RESULT}
        0
        PARENT_SCOPE)
  else()
    set(${OUT_RESULT}
        ${${INPUT_VERSION}}
        PARENT_SCOPE)
  endif()
endfunction()

if(WIN32 AND NOT MINGW)
  string(TIMESTAMP GOOSE_COPYRIGHT_YEAR "%Y")
  ensure_variable_is_number(GOOSE_MAJOR_VERSION RC_MAJOR_VERSION)
  ensure_variable_is_number(GOOSE_MINOR_VERSION RC_MINOR_VERSION)
  ensure_variable_is_number(GOOSE_PATCH_VERSION RC_PATCH_VERSION)
  ensure_variable_is_number(GOOSE_DEV_ITERATION RC_DEV_ITERATION)

  set(CMAKE_RC_FLAGS
      "${CMAKE_RC_FLAGS} -D GOOSE_VERSION=\"${GOOSE_VERSION}\"")
  set(CMAKE_RC_FLAGS
      "${CMAKE_RC_FLAGS} -D GOOSE_MAJOR_VERSION=\"${RC_MAJOR_VERSION}\"")
  set(CMAKE_RC_FLAGS
      "${CMAKE_RC_FLAGS} -D GOOSE_MINOR_VERSION=\"${RC_MINOR_VERSION}\"")
  set(CMAKE_RC_FLAGS
      "${CMAKE_RC_FLAGS} -D GOOSE_PATCH_VERSION=\"${RC_PATCH_VERSION}\"")
  set(CMAKE_RC_FLAGS
      "${CMAKE_RC_FLAGS} -D GOOSE_DEV_ITERATION=\"${RC_DEV_ITERATION}\"")
  set(CMAKE_RC_FLAGS
      "${CMAKE_RC_FLAGS} -D GOOSE_COPYRIGHT_YEAR=\"${GOOSE_COPYRIGHT_YEAR}\"")
  target_sources(shell PRIVATE rc/goose.rc)
endif()

set_target_properties(shell PROPERTIES OUTPUT_NAME goosedb)
set_target_properties(shell PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                       ${PROJECT_BINARY_DIR})

install(TARGETS shell RUNTIME DESTINATION "${INSTALL_BIN_DIR}")
