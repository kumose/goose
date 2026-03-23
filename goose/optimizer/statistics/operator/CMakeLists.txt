add_library_unity(
  goose_optimizer_statistics_op
  OBJECT
  propagate_aggregate.cc
  propagate_cross_product.cc
  propagate_filter.cc
  propagate_get.cc
  propagate_limit.cc
  propagate_join.cc
  propagate_order.cc
  propagate_projection.cc
  propagate_set_operation.cc
  propagate_window.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_optimizer_statistics_op>
    PARENT_SCOPE)
