add_library_unity(test_table_function OBJECT table_in_out.cc
                  table_bind_replace.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_table_function>
    PARENT_SCOPE)
