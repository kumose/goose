add_library_unity(
  test_sql_interquery_parallelism
  OBJECT
  concurrent_attach_detach.cc
  concurrent_checkpoint.cc
  test_concurrentappend.cc
  test_concurrentdelete.cc
  test_concurrent_dependencies.cc
  test_concurrent_index.cc
  test_concurrentupdate.cc
  test_concurrent_sequence.cc
  test_concurrent_prepared.cc
  test_default_catalog.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_sql_interquery_parallelism>
    PARENT_SCOPE)
