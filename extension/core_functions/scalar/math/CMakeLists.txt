add_library_unity(goose_core_functions_math OBJECT numeric.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_math>
    PARENT_SCOPE)
