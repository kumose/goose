add_library_unity(
        goose_catalog_default_entries
  OBJECT
  default_functions.cc
  default_generator.cc
  default_schemas.cc
  default_table_functions.cc
  default_types.cc
  default_views.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_catalog_default_entries>
    PARENT_SCOPE)
