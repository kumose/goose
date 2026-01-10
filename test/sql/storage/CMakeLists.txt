add_subdirectory(catalog)
add_library_unity(
  test_sql_storage
  OBJECT
  test_buffer_manager.cc
  test_checksum.cc
  test_storage.cc
  test_database_size.cc
  wal_torn_write.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_sql_storage>
    PARENT_SCOPE)
