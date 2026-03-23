add_library_unity(goose_func_string_regexp OBJECT regexp_util.cc
                  regexp_extract_all.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_string_regexp>
    PARENT_SCOPE)
