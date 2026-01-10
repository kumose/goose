add_library_unity(goose_core_functions_union OBJECT union_extract.cc
                  union_value.cc union_tag.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_union>
    PARENT_SCOPE)
