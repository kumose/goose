add_library_unity(goose_func_seq OBJECT nextval.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_seq>
    PARENT_SCOPE)
