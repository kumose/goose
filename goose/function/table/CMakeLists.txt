add_subdirectory(system)
add_subdirectory(version)
add_subdirectory(arrow)
add_library_unity(
  goose_func_table
  OBJECT
  arrow.cc
  arrow_conversion.cc
  checkpoint.cc
  direct_file_reader.cc
  glob.cc
  query_function.cc
  range.cc
  repeat.cc
  repeat_row.cc
  copy_csv.cc
  read_csv.cc
  read_goose.cc
  sniff_csv.cc
  read_file.cc
  system_functions.cc
  summary.cc
  table_scan.cc
  unnest.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_table>
    PARENT_SCOPE)
