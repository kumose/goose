add_library_unity(
  goose_storage_checkpoint OBJECT row_group_writer.cc table_data_writer.cc
  table_data_reader.cc write_overflow_strings_to_disk.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_storage_checkpoint>
    PARENT_SCOPE)
