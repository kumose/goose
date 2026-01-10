add_library_unity(
  goose_query_node
  OBJECT
  recursive_cte_node.cc
  cte_node.cc
  select_node.cc
  set_operation_node.cc
  statement_node.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_query_node>
    PARENT_SCOPE)
