if(NOT CLANG_TIDY)
  add_subdirectory(adbc)
endif()

add_subdirectory(arrow)
add_subdirectory(crypto)
add_subdirectory(enums)
add_subdirectory(exception)
add_subdirectory(multi_file)
add_subdirectory(operator)
add_subdirectory(progress_bar)
add_subdirectory(tree_renderer)
add_subdirectory(row_operations)
add_subdirectory(serializer)
add_subdirectory(sort)
add_subdirectory(types)
add_subdirectory(value_operations)
add_subdirectory(vector_operations)

if(${EXIT_TIME_DESTRUCTORS_WARNING})
  set(CMAKE_CXX_FLAGS_DEBUG
      "${CMAKE_CXX_FLAGS_DEBUG} -Wno-exit-time-destructors")
endif()

if(DEBUG_STACKTRACE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGOOSE_DEBUG_STACKTRACE")
endif()

add_library_unity(
        goose_common
  OBJECT
  allocator.cc
  assert.cc
  bind_helpers.cc
  box_renderer.cc
  cgroups.cc
  csv_writer.cc
  complex_json.cc
  compressed_file_system.cc
  constants.cc
  checksum.cc
  encryption_functions.cc
  encryption_key_manager.cc
  encryption_state.cc
  exception.cc
  exception_format_value.cc
  extra_type_info.cc
  file_buffer.cc
  file_system.cc
  filename_pattern.cc
  fsst.cc
  gzip_file_system.cc
  hive_partitioning.cc
  pipe_file_system.cc
  local_file_system.cc
  error_data.cc
  opener_file_system.cc
  printer.cc
  radix_partitioning.cc
  re2_regex.cc
  random_engine.cc
  stacktrace.cc
  string_util.cc
  enum_util.cc
  symbols.cc
  render_tree.cc
  thread_util.cc
  tree_renderer.cc
  types.cc
  bignum.cc
  virtual_file_system.cc
  windows_util.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_common>
    PARENT_SCOPE)
