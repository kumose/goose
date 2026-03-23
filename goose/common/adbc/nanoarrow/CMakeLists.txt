add_library_unity(goose_adbc_nanoarrow OBJECT metadata.cc schema.cc
                  allocator.cc single_batch_array_stream.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_adbc_nanoarrow>
    PARENT_SCOPE)
