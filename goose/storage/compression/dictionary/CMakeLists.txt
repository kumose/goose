add_library_unity(goose_storage_compression_dictionary OBJECT common.cc
                  analyze.cc compression.cc decompression.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_storage_compression_dictionary>
    PARENT_SCOPE)
