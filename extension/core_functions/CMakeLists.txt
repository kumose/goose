cmake_minimum_required(VERSION 3.15...3.29)

project(CoreFunctionsExtension)

include_directories(include)
add_subdirectory(aggregate)
add_subdirectory(scalar)


set(CORE_FUNCTION_FILES ${CORE_FUNCTION_FILES} core_functions_extension.cc
                        function_list.cc lambda_functions.cc)

build_static_extension(core_functions ${CORE_FUNCTION_FILES})
set(PARAMETERS "-warnings")
build_loadable_extension(core_functions ${PARAMETERS} ${CORE_FUNCTION_FILES})

install(
  TARGETS core_functions_extension
  EXPORT "${GOOSE_EXPORT_SET}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")
