add_library_unity(
  goose_operator_persistent
  OBJECT
  csv_rejects_table.cc
  physical_batch_copy_to_file.cc
  physical_batch_insert.cc
  physical_copy_database.cc
  physical_copy_to_file.cc
  physical_delete.cc
  physical_export.cc
  physical_insert.cc
  physical_merge_into.cc
  physical_update.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_operator_persistent>
    PARENT_SCOPE)
