add_library_unity(goose_value_operations OBJECT comparison_operations.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_value_operations>
    PARENT_SCOPE)
