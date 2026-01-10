add_library_unity(goose_func_pragma OBJECT pragma_functions.cc
                  pragma_queries.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_pragma>
    PARENT_SCOPE)
