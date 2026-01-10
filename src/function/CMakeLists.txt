add_subdirectory(aggregate)
add_subdirectory(cast)
add_subdirectory(pragma)
add_subdirectory(scalar)
add_subdirectory(table)
add_subdirectory(window)
add_subdirectory(variant)

if(GENERATE_EXTENSION_ENTRIES)
  add_definitions(-DGENERATE_EXTENSION_ENTRIES)
endif()

add_library_unity(
  goose_function
  OBJECT
  aggregate_function.cc
  built_in_functions.cc
  cast_rules.cc
  compression_config.cc
  copy_function.cc
  encoding_function.cc
  function.cc
  function_binder.cc
  function_list.cc
  function_set.cc
  pragma_function.cc
  macro_function.cc
  register_function_list.cc
  scalar_macro_function.cc
  table_macro_function.cc
  scalar_function.cc
  table_function.cc
  udf_function.cc
  copy_blob.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_function>
    PARENT_SCOPE)
