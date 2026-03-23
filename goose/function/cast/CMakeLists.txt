add_subdirectory(union)
add_subdirectory(variant)

add_library_unity(
  goose_func_cast
  OBJECT
  array_casts.cc
  blob_cast.cc
  bit_cast.cc
  cast_function_set.cc
  decimal_cast.cc
  default_casts.cc
  enum_casts.cc
  geo_casts.cc
  list_casts.cc
  map_cast.cc
  numeric_casts.cc
  pointer_cast.cc
  string_cast.cc
  struct_cast.cc
  time_casts.cc
  union_casts.cc
  uuid_casts.cc
  bignum_casts.cc
  nested_to_varchar_cast.cc
  vector_cast_helpers.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_cast>
    PARENT_SCOPE)
