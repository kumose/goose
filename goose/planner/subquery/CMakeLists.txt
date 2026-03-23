add_library_unity(
  goose_planner_subquery OBJECT flatten_dependent_join.cc
  has_correlated_expressions.cc rewrite_correlated_expressions.cc
  rewrite_cte_scan.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_planner_subquery>
    PARENT_SCOPE)
