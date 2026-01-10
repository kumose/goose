add_extension_definitions()

add_definitions(-DGOOSE_ROOT_DIRECTORY="${PROJECT_SOURCE_DIR}"
                -DGOOSE_BUILD_DIRECTORY="${PROJECT_BINARY_DIR}")

set(SQLITE_TEST_RUNNER_SOURCES
    result_helper.cc sqllogic_command.cc sqllogic_test_runner.cc
    sqllogic_parser.cc test_sqllogictest.cc sqllogic_test_logger.cc)

add_library_unity(test_sqlite OBJECT ${SQLITE_TEST_RUNNER_SOURCES})
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_sqlite>
    PARENT_SCOPE)
