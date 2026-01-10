add_library_unity(goose_core_functions_debug OBJECT vector_type.cc sleep.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_debug>
    PARENT_SCOPE)
