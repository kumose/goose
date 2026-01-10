add_library_unity(
  goose_core_functions_generic
  OBJECT
  alias.cc
  binning.cc
  cast_to_type.cc
  can_implicitly_cast.cc
  current_setting.cc
  hash.cc
  least.cc
  replace_type.cc
  stats.cc
  typeof.cc
  system_functions.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_generic>
    PARENT_SCOPE)
