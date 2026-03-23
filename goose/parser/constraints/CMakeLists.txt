add_library_unity(
  goose_constraints OBJECT check_constraint.cc not_null_constraint.cc
  unique_constraint.cc foreign_key_constraint.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_constraints>
    PARENT_SCOPE)
