add_library_unity(
  test_appender
  OBJECT
  test_appender_abort.cc
  test_appender.cc
  test_appender_query.cc
  test_concurrent_append.cc
  test_appender_transactions.cc
  test_nested_appender.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_appender>
    PARENT_SCOPE)
