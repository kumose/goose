add_subdirectory(distributive)

add_library_unity(goose_func_aggr OBJECT sorted_aggregate_function.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_aggr>
    PARENT_SCOPE)
