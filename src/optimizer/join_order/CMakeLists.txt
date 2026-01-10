add_library_unity(
  goose_optimizer_join_order
  OBJECT
  query_graph.cc
  join_relation_set.cc
  join_node.cc
  join_order_optimizer.cc
  cardinality_estimator.cc
  cost_model.cc
  plan_enumerator.cc
  relation_manager.cc
  query_graph_manager.cc
  relation_statistics_helper.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_optimizer_join_order>
    PARENT_SCOPE)
