add_library_unity(
  goose_csv_scanner
  OBJECT
  base_scanner.cc
  column_count_scanner.cc
  csv_schema.cc
  scanner_boundary.cc
  skip_scanner.cc
  string_value_scanner.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_csv_scanner>
    PARENT_SCOPE)
