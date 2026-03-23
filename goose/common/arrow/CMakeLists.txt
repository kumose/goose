add_library_unity(
        goose_common_arrow
  OBJECT
  arrow_appender.cc
  arrow_converter.cc
  arrow_type_extension.cc
  arrow_merge_event.cc
  arrow_query_result.cc
  arrow_util.cc
  arrow_wrapper.cc
  physical_arrow_collector.cc
  physical_arrow_batch_collector.cc
  schema_metadata.cc)
add_subdirectory(appender)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_common_arrow>
    PARENT_SCOPE)
