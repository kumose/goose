add_library_unity(test_sql_adbc OBJECT test_adbc.cc driver_manager.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_sql_adbc>
    PARENT_SCOPE)
