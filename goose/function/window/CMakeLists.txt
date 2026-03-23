add_library_unity(
  goose_func_window
  OBJECT
  window_aggregate_function.cc
  window_aggregate_states.cc
  window_aggregator.cc
  window_boundaries_state.cc
  window_collection.cc
  window_constant_aggregator.cc
  window_custom_aggregator.cc
  window_distinct_aggregator.cc
  window_executor.cc
  window_index_tree.cc
  window_merge_sort_tree.cc
  window_naive_aggregator.cc
  window_rank_function.cc
  window_rownumber_function.cc
  window_segment_tree.cc
  window_shared_expressions.cc
  window_token_tree.cc
  window_value_function.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_window>
    PARENT_SCOPE)
