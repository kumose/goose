add_library_unity(
  goose_core_functions_regression
  OBJECT
  regr_sxy.cc
  regr_intercept.cc
  regr_count.cc
  regr_r2.cc
  regr_avg.cc
  regr_slope.cc
  regr_sxx_syy.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_regression>
    PARENT_SCOPE)
