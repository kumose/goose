add_library_unity(
  goose_core_functions_distributive
  OBJECT
  kurtosis.cc
  string_agg.cc
  sum.cc
  arg_min_max.cc
  approx_count.cc
  skew.cc
  bitagg.cc
  bitstring_agg.cc
  product.cc
  bool.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_distributive>
    PARENT_SCOPE)
