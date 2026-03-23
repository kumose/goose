add_library_unity(
  goose_expression_executor
  OBJECT
  execute_between.cc
  execute_case.cc
  execute_cast.cc
  execute_comparison.cc
  execute_conjunction.cc
  execute_constant.cc
  execute_function.cc
  execute_operator.cc
  execute_parameter.cc
  execute_reference.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_expression_executor>
    PARENT_SCOPE)
