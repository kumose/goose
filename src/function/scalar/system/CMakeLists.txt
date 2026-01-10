add_library_unity(
  goose_func_system
  OBJECT
  aggregate_export.cc
  write_log.cc
  current_transaction_id.cc
  current_connection_id.cc
  current_query_id.cc
  parse_log_message.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_system>
    PARENT_SCOPE)
