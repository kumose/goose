add_library_unity(
  test_common
  OBJECT
  test_cast.cc
  test_checksum.cc
  test_file_system.cc
  test_caching_file_system_wrapper.cc
  test_hyperlog.cc
  test_lru_cache.cc
  test_numeric_cast.cc
  test_parse_logical_type.cc
  test_utf.cc
  test_storage_fuzz.cc
  test_strftime.cc
  test_string_util.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_common>
    PARENT_SCOPE)
