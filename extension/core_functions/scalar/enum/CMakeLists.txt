add_library_unity(goose_core_functions_enum OBJECT enum_functions.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_enum>
    PARENT_SCOPE)
