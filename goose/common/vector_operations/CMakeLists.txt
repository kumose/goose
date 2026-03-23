add_library(
  goose_vector_operations OBJECT
  boolean_operators.cc
  vector_cast.cc
  comparison_operators.cc
  vector_copy.cc
  generators.cc
  vector_hash.cc
  vector_storage.cc
  null_operations.cc
  numeric_inplace_operators.cc
  is_distinct_from.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_vector_operations>
    PARENT_SCOPE)
