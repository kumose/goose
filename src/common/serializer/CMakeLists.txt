add_library_unity(
  goose_common_serializer
  OBJECT
  binary_serializer.cc
  binary_deserializer.cc
  buffered_file_reader.cc
  buffered_file_writer.cc
  memory_stream.cc
  serializer.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_common_serializer>
    PARENT_SCOPE)
