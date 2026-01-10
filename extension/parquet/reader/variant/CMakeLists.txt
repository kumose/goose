add_library_unity(goose_parquet_reader_variant OBJECT
                  variant_binary_decoder.cc variant_shredded_conversion.cc)

set(PARQUET_EXTENSION_FILES
    ${PARQUET_EXTENSION_FILES} $<TARGET_OBJECTS:goose_parquet_reader_variant>
    PARENT_SCOPE)
