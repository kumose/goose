add_library_unity(
  goose_logging
  OBJECT
  log_manager.cc
  log_storage.cc
  log_types.cc
  logger.cc
  logging.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_logging>
    PARENT_SCOPE)
