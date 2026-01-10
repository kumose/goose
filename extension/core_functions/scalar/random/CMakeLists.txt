add_library_unity(goose_core_functions_random OBJECT random.cc setseed.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_random>
    PARENT_SCOPE)
