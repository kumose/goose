add_library_unity(
  goose_operator_aggregate
  OBJECT
  aggregate_object.cc
  distinct_aggregate_data.cc
  physical_hash_aggregate.cc
  grouped_aggregate_data.cc
  physical_partitioned_aggregate.cc
  physical_perfecthash_aggregate.cc
  physical_ungrouped_aggregate.cc
  physical_window.cc
  physical_streaming_window.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_operator_aggregate>
    PARENT_SCOPE)
