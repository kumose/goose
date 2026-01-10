add_library_unity(test_memory_leak OBJECT test_appender.cc
                  test_temporary_tables.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_memory_leak>
    PARENT_SCOPE)
