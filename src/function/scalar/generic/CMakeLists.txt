add_library_unity(goose_func_generic_main OBJECT constant_or_null.cc
                  error.cc getvariable.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_generic_main>
    PARENT_SCOPE)
