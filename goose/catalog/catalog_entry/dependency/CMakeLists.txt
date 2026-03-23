add_library_unity(goose_catalog_entries_dependency OBJECT dependency_entry.cc
                  dependency_subject_entry.cc dependency_dependent_entry.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_catalog_entries_dependency>
    PARENT_SCOPE)
