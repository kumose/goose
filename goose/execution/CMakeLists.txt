add_subdirectory(expression_executor)
add_subdirectory(nested_loop_join)
add_subdirectory(operator)
add_subdirectory(physical_plan)
add_subdirectory(index)
add_subdirectory(sample)

add_library_unity(
  goose_execution
  OBJECT
  adaptive_filter.cc
  aggregate_hashtable.cc
  base_aggregate_hashtable.cc
  column_binding_resolver.cc
  expression_executor.cc
  expression_executor_state.cc
  join_hashtable.cc
  perfect_aggregate_hashtable.cc
  physical_operator.cc
  physical_plan_generator.cc
  radix_partitioned_hashtable.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_execution>
    PARENT_SCOPE)
