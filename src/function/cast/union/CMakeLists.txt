add_library_unity(goose_union_cast OBJECT from_struct.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_union_cast>
    PARENT_SCOPE)
