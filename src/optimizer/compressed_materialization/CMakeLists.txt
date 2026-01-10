add_library_unity(
  goose_optimizer_compressed_materialization OBJECT compress_aggregate.cc
  compress_comparison_join.cc compress_distinct.cc compress_order.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES}
    $<TARGET_OBJECTS:goose_optimizer_compressed_materialization>
    PARENT_SCOPE)
