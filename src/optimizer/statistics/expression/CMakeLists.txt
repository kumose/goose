add_library_unity(
  goose_optimizer_statistics_expr
  OBJECT
  propagate_aggregate.cc
  propagate_between.cc
  propagate_case.cc
  propagate_cast.cc
  propagate_comparison.cc
  propagate_conjunction.cc
  propagate_constant.cc
  propagate_columnref.cc
  propagate_function.cc
  propagate_operator.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_optimizer_statistics_expr>
    PARENT_SCOPE)
