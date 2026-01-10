add_library_unity(goose_core_functions_struct OBJECT struct_insert.cc
                  struct_update.cc struct_keys.cc struct_values.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_struct>
    PARENT_SCOPE)
