add_library_unity(goose_core_functions_array OBJECT array_functions.cc
                  array_value.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_array>
    PARENT_SCOPE)
