add_definitions(-DGOOSE_ROOT_DIRECTORY="${PROJECT_SOURCE_DIR}"
                -DGOOSE_BUILD_DIRECTORY="${PROJECT_BINARY_DIR}")

set(GOOSE_TEST_HELPERS_UNITS test_helpers.cc capi_tester.cc pid.cc
                              test_config.cc)

add_library(test_helpers STATIC ${GOOSE_TEST_HELPERS_UNITS})
