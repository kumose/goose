add_library_unity(goose_storage_metadata OBJECT metadata_manager.cc
                  metadata_reader.cc metadata_writer.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_storage_metadata>
    PARENT_SCOPE)
