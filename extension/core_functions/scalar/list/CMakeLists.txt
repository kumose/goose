add_library_unity(
  goose_core_functions_list
  OBJECT
  flatten.cc
  list_transform.cc
  range.cc
  list_value.cc
  list_filter.cc
  list_has_any_or_all.cc
  list_aggregates.cc
  list_distance.cc
  array_slice.cc
  list_sort.cc
  list_reduce.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_list>
    PARENT_SCOPE)
