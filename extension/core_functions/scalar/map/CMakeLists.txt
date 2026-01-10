add_library_unity(
  goose_core_functions_map
  OBJECT
  map_keys_values.cc
  map_extract.cc
  map_from_entries.cc
  map_entries.cc
  map.cc
  map_concat.cc
  switch.cc
  cardinality.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_map>
    PARENT_SCOPE)
