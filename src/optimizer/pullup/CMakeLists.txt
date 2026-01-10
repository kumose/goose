add_library_unity(
  goose_optimizer_pullup
  OBJECT
  pullup_filter.cc
  pullup_projection.cc
  pullup_from_left.cc
  pullup_both_side.cc
  pullup_set_operation.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_optimizer_pullup>
    PARENT_SCOPE)
