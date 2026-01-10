add_library_unity(goose_core_functions_operators OBJECT bitwise.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_operators>
    PARENT_SCOPE)
