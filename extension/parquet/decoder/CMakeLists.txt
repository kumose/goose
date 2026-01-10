add_library_unity(
  goose_parquet_decoders
  OBJECT
  byte_stream_split_decoder.cc
  delta_binary_packed_decoder.cc
  delta_byte_array_decoder.cc
  delta_length_byte_array_decoder.cc
  dictionary_decoder.cc
  rle_decoder.cc)
set(PARQUET_EXTENSION_FILES
    ${PARQUET_EXTENSION_FILES} $<TARGET_OBJECTS:goose_parquet_decoders>
    PARENT_SCOPE)
