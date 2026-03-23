add_library_unity(
  goose_storage_buffer
  OBJECT
  buffer_handle.cc
  block_handle.cc
  block_manager.cc
  buffer_pool.cc
  buffer_pool_reservation.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_storage_buffer>
    PARENT_SCOPE)
