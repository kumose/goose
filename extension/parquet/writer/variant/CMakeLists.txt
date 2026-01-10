add_library_unity(goose_parquet_writer_variant OBJECT convert_variant.cc
                  analyze_variant.cc)

set(PARQUET_EXTENSION_FILES
    ${PARQUET_EXTENSION_FILES} $<TARGET_OBJECTS:goose_parquet_writer_variant>
    PARENT_SCOPE)
