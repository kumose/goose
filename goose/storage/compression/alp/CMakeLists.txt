add_library_unity(goose_storage_compression_alp OBJECT alp_constants.cc
                  alp.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_storage_compression_alp>
    PARENT_SCOPE)
