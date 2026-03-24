
# Allows loading tests from registered extensions
include_directories("${PROJECT_BINARY_DIR}/codegen/include/")
add_definitions(-DGENERATED_EXTENSION_HEADERS=1)
add_definitions(-DGOOSE_MAJOR_VERSION=${GOOSE_MAJOR_VERSION})
add_definitions(-DGOOSE_MINOR_VERSION=${GOOSE_MINOR_VERSION})
add_definitions(-DGOOSE_PATCH_VERSION=${GOOSE_PATCH_VERSION})

add_subdirectory(extension)

if(NOT WIN32 AND NOT SUN)
  if(${ENABLE_UNITTEST_CPP_TESTS})
    add_subdirectory(persistence)
  endif()
endif()

set(UNITTEST_ROOT_DIRECTORY
    ""
    CACHE
      STRING
      "The unittest root directory (defaults to the project source directory)")
if(DEFINED UNITTEST_ROOT_DIRECTORY AND NOT UNITTEST_ROOT_DIRECTORY STREQUAL "")
  add_definitions(-DGOOSE_ROOT_DIRECTORY="${UNITTEST_ROOT_DIRECTORY}")
else()
  add_definitions(-DGOOSE_ROOT_DIRECTORY="${PROJECT_SOURCE_DIR}")
endif()

add_executable(unittest unittest.cc ${ALL_OBJECT_FILES})

target_link_libraries(unittest goose_static test_helpers)

link_extension_libraries(unittest "")
