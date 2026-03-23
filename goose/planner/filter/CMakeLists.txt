add_library_unity(
  goose_planner_filter
  OBJECT
  bloom_filter.cc
  conjunction_filter.cc
  constant_filter.cc
  dynamic_filter.cc
  expression_filter.cc
  in_filter.cc
  null_filter.cc
  struct_filter.cc
  optional_filter.cc
  selectivity_optional_filter.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_planner_filter>
    PARENT_SCOPE)
