add_subdirectory(dependency)

add_library_unity(
        goose_catalog_entries
        OBJECT
        copy_function_catalog_entry.cc
        goose_index_entry.cc
        goose_schema_entry.cc
        goose_table_entry.cc
        type_catalog_entry.cc
        index_catalog_entry.cc
        macro_catalog_entry.cc
        pragma_function_catalog_entry.cc
        schema_catalog_entry.cc
        sequence_catalog_entry.cc
        table_catalog_entry.cc
        column_dependency_manager.cc
        scalar_function_catalog_entry.cc
        table_function_catalog_entry.cc
        view_catalog_entry.cc)
set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_catalog_entries>
        PARENT_SCOPE)
