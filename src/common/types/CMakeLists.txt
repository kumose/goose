add_subdirectory(column)
add_subdirectory(row)
add_subdirectory(variant)

if(${EXIT_TIME_DESTRUCTORS_WARNING})
  set(CMAKE_CXX_FLAGS_DEBUG
      "${CMAKE_CXX_FLAGS_DEBUG} -Wno-exit-time-destructors")
endif()

add_library_unity(
  goose_common_types
  OBJECT
  batched_data_collection.cc
  bit.cc
  blob.cc
  cast_helpers.cc
  conflict_manager.cc
  conflict_info.cc
  data_chunk.cc
  date.cc
  decimal.cc
  hash.cc
  hugeint.cc
  uhugeint.cc
  uuid.cc
  hyperloglog.cc
  interval.cc
  list_segment.cc
  selection_vector.cc
  string_heap.cc
  string_type.cc
  timestamp.cc
  time.cc
  validity_mask.cc
  value.cc
  bignum.cc
  vector_buffer.cc
  vector.cc
  vector_cache.cc
  vector_constants.cc
  geometry.cc
  geometry_crs.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_common_types>
    PARENT_SCOPE)
