add_library_unity(goose_operator_filter OBJECT physical_filter.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_operator_filter>
    PARENT_SCOPE)
