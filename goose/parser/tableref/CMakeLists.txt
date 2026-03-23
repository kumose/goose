add_library_unity(
  goose_parser_tableref
  OBJECT
  at_clause.cc
  basetableref.cc
  bound_ref_wrapper.cc
  delimgetref.cc
  emptytableref.cc
  expressionlistref.cc
  column_data_ref.cc
  joinref.cc
  pivotref.cc
  showref.cc
  subqueryref.cc
  table_function.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_parser_tableref>
    PARENT_SCOPE)
