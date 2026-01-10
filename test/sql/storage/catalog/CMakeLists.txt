add_library_unity(test_sql_storage_catalog OBJECT test_storage_sequences.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_sql_storage_catalog>
    PARENT_SCOPE)
