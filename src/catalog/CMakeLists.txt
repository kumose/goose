add_subdirectory(catalog_entry)
add_subdirectory(default)

if (${BUILD_CORE_FUNCTIONS_EXTENSION})

else ()
    add_definitions(-DDISABLE_CORE_FUNCTIONS_EXTENSION=1)
endif ()
add_library_unity(
        goose_catalog
        OBJECT
        catalog_entry.cc
        catalog_entry_retriever.cc
        catalog.cc
        catalog_search_path.cc
        catalog_set.cc
        catalog_transaction.cc
        goose_catalog.cc
        dependency_manager.cc
        dependency_list.cc
        dependency_catalog_set.cc
        entry_lookup_info.cc
        similar_catalog_entry.cc)
set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_catalog>
        PARENT_SCOPE)
