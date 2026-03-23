add_library_unity(goose_func_compressed_materialization OBJECT
                  compress_integral.cc compress_string.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_compressed_materialization>
    PARENT_SCOPE)
