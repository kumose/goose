add_library_unity(goose_func_date OBJECT strftime.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_date>
    PARENT_SCOPE)
