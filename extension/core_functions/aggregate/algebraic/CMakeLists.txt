add_library_unity(goose_core_functions_algebraic OBJECT corr.cc stddev.cc
                  avg.cc covar.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_algebraic>
    PARENT_SCOPE)
