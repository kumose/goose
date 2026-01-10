add_library_unity(
  goose_storage_compression_chimp
  OBJECT
  bit_reader.cc
  chimp_constants.cc
  flag_buffer.cc
  leading_zero_buffer.cc
  chimp.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_storage_compression_chimp>
    PARENT_SCOPE)
