add_library_unity(
  goose_parquet_writers
  OBJECT
  array_column_writer.cc
  boolean_column_writer.cc
  decimal_column_writer.cc
  enum_column_writer.cc
  list_column_writer.cc
  primitive_column_writer.cc
  struct_column_writer.cc)

add_subdirectory(variant)

set(PARQUET_EXTENSION_FILES
    ${PARQUET_EXTENSION_FILES} $<TARGET_OBJECTS:goose_parquet_writers>
    PARENT_SCOPE)
