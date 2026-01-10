if(${EXIT_TIME_DESTRUCTORS_WARNING})
  set(CMAKE_CXX_FLAGS_DEBUG
      "${CMAKE_CXX_FLAGS_DEBUG} -Wno-exit-time-destructors")
endif()

add_library_unity(
  goose_common_types_column
  OBJECT
  column_data_allocator.cc
  column_data_collection.cc
  column_data_collection_segment.cc
  column_data_consumer.cc
  partitioned_column_data.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_common_types_column>
    PARENT_SCOPE)
