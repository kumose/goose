add_library_unity(goose_core_functions_nested OBJECT binned_histogram.cc
                  list.cc histogram.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_nested>
    PARENT_SCOPE)
