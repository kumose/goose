add_library_unity(
  goose_parquet_readers
  OBJECT
  decimal_column_reader.cc
  expression_column_reader.cc
  list_column_reader.cc
  row_number_column_reader.cc
  string_column_reader.cc
  struct_column_reader.cc
  variant_column_reader.cc)

add_subdirectory(variant)

set(PARQUET_EXTENSION_FILES
    ${PARQUET_EXTENSION_FILES} $<TARGET_OBJECTS:goose_parquet_readers>
    PARENT_SCOPE)
