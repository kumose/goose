add_subdirectory(art)
add_library_unity(
  goose_execution_index
  OBJECT
  fixed_size_allocator.cc
  fixed_size_buffer.cc
  unbound_index.cc
  index_type_set.cc
  bound_index.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_execution_index>
    PARENT_SCOPE)
