if(${EXIT_TIME_DESTRUCTORS_WARNING})
  set(CMAKE_CXX_FLAGS_DEBUG
      "${CMAKE_CXX_FLAGS_DEBUG} -Wno-exit-time-destructors")
endif()

add_library_unity(
  goose_common_types_row
  OBJECT
  partitioned_tuple_data.cc
  tuple_data_allocator.cc
  tuple_data_collection.cc
  tuple_data_iterator.cc
  tuple_data_layout.cc
  tuple_data_scatter_gather.cc
  tuple_data_segment.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_common_types_row>
    PARENT_SCOPE)
