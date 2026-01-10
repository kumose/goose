add_library_unity(
  goose_transformer_helpers
  OBJECT
  transform_alias.cc
  transform_cte.cc
  transform_groupby.cc
  transform_orderby.cc
  transform_sample.cc
  transform_typename.cc
  nodetype_to_string.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_transformer_helpers>
    PARENT_SCOPE)
