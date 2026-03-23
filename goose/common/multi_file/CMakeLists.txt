add_library_unity(
  goose_common_multi_file
  OBJECT
  base_file_reader.cc
  multi_file_function.cc
  multi_file_list.cc
  multi_file_reader.cc
  multi_file_column_mapper.cc
  union_by_name.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_common_multi_file>
    PARENT_SCOPE)
