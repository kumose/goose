if(NOT CLANG_TIDY)
  add_subdirectory(adbc)
endif()

if(${DISABLE_THREADS})
  add_definitions(-DGOOSE_NO_THREADS)
endif()

add_subdirectory(capi)
add_subdirectory(udf_function)

set(TEST_API_OBJECTS
    test_api.cc
    test_config.cc
    test_custom_allocator.cc
    test_extension_setting_autoload.cc
    test_instance_cache.cc
    test_storage_extension_alias.cc
    test_results.cc
    test_reset.cc
    test_get_table_names.cc
    test_prepared_api.cc
    test_table_info.cc
    test_appender_api.cc
    test_lifecycle_hooks.cc
    test_pending_query.cc
    test_plan_serialization.cc
    test_relation_api.cc
    test_query_profiler.cc
    test_dbdir.cc
    test_pending_with_parameters.cc
    test_progress_bar.cc
    test_uuid.cc
    test_insertion_order_preserving_map.cc
    test_bignum.cc
    test_threads.cc
    test_windows_header_compatibility.cc
    test_windows_unicode_path.cc
    test_object_cache.cc)

if(NOT WIN32)
  set(TEST_API_OBJECTS ${TEST_API_OBJECTS} test_read_only.cc)
endif()

if(GOOSE_EXTENSION_TPCH_SHOULD_LINK)
  include_directories(../../extension/tpch/include)
  set(TEST_API_OBJECTS
      ${TEST_API_OBJECTS} test_tpch_with_relations.cc
      test_tpch_with_streaming.cc
      serialized_plans/test_plan_serialization_bwc.cc)
endif()

add_library(test_api OBJECT ${TEST_API_OBJECTS})
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_api>
    PARENT_SCOPE)
