add_library_unity(
  goose_operator_scan
  OBJECT
  physical_column_data_scan.cc
  physical_dummy_scan.cc
  physical_empty_result.cc
  physical_expression_scan.cc
  physical_positional_scan.cc
  physical_table_scan.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_operator_scan>
    PARENT_SCOPE)
