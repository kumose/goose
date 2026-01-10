add_library_unity(
  goose_operator_csv_table_function OBJECT csv_file_scanner.cc
  global_csv_state.cc csv_multi_file_info.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_operator_csv_table_function>
    PARENT_SCOPE)
