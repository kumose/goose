add_subdirectory(chimp)
add_subdirectory(alp)
add_subdirectory(roaring)
add_subdirectory(dictionary)
add_subdirectory(dict_fsst)

add_library_unity(
  goose_storage_compression
  OBJECT
  numeric_constant.cc
  fixed_size_uncompressed.cc
  rle.cc
  dictionary_compression.cc
  string_uncompressed.cc
  uncompressed.cc
  validity_uncompressed.cc
  bitpacking.cc
  bitpacking_hugeint.cc
  patas.cc
  zstd.cc
  alprd.cc
  fsst.cc
  dict_fsst.cc
  empty_validity.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_storage_compression>
    PARENT_SCOPE)
