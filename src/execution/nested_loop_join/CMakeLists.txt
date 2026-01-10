add_library_unity(goose_nested_loop_join OBJECT nested_loop_join_inner.cc
                  nested_loop_join_mark.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_nested_loop_join>
    PARENT_SCOPE)
