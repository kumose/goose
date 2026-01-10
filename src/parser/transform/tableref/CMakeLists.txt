add_library_unity(
  goose_transformer_tableref
  OBJECT
  transform_base_tableref.cc
  transform_join.cc
  transform_from.cc
  transform_pivot.cc
  transform_subquery.cc
  transform_table_function.cc
  transform_tableref.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_transformer_tableref>
    PARENT_SCOPE)
