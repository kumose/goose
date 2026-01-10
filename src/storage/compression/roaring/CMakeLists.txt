add_library_unity(
  goose_storage_compression_roaring
  OBJECT
  common.cc
  metadata.cc
  analyze.cc
  compress.cc
  scan.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_storage_compression_roaring>
    PARENT_SCOPE)
