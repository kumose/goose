add_library_unity(
  goose_bind_query_node
  OBJECT
  bind_select_node.cc
  bind_setop_node.cc
  bind_recursive_cte_node.cc
  bind_cte_node.cc
  bind_statement_node.cc
  bind_table_macro_node.cc
  plan_query_node.cc
  plan_select_node.cc
  plan_setop.cc
  plan_subquery.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_bind_query_node>
    PARENT_SCOPE)
