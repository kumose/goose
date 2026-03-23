add_library_unity(
  goose_storage_statistics
  OBJECT
  base_statistics.cc
  column_statistics.cc
  distinct_statistics.cc
  array_stats.cc
  list_stats.cc
  numeric_stats.cc
  segment_statistics.cc
  string_stats.cc
  struct_stats.cc
  geometry_stats.cc
  variant_stats.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_storage_statistics>
    PARENT_SCOPE)
