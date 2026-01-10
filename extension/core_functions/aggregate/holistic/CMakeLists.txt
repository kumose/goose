add_library_unity(
  goose_core_functions_holistic
  OBJECT
  approx_top_k.cc
  quantile.cc
  reservoir_quantile.cc
  mad.cc
  approximate_quantile.cc
  mode.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_holistic>
    PARENT_SCOPE)
