add_library_unity(
  goose_operator_schema
  OBJECT
  physical_alter.cc
  physical_attach.cc
  physical_create_index.cc
  physical_create_schema.cc
  physical_create_type.cc
  physical_create_sequence.cc
  physical_create_table.cc
  physical_create_view.cc
  physical_create_function.cc
  physical_detach.cc
  physical_drop.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_operator_schema>
    PARENT_SCOPE)
