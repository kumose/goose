add_library_unity(goose_function_map OBJECT map_contains.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_function_map>
    PARENT_SCOPE)
