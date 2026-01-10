set(PARAMETERS "-warnings")

build_loadable_extension_directory(
  loadable_extension_demo
  "CPP"
  test/extension
  "default-version"
  ""
  ${PARAMETERS}
  loadable_extension_demo.cc)

if(NOT WIN32 AND NOT SUN)
  add_definitions(-DGOOSE_BUILD_DIRECTORY="${PROJECT_BINARY_DIR}")

  build_loadable_extension_directory(
    loadable_extension_optimizer_demo
    "CPP"
    test/extension
    "default-version"
    ""
    ${PARAMETERS}
    ../extension/loadable_extension_optimizer_demo.cc)

  if(${ENABLE_UNITTEST_CPP_TESTS} AND NOT (${ENABLE_THREAD_SANITIZER}))
    set(TEST_EXT_OBJECTS test_remote_optimizer.cc)

    add_library_unity(test_extensions OBJECT ${TEST_EXT_OBJECTS})
    set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_extensions>
        PARENT_SCOPE)
  endif()
endif()
