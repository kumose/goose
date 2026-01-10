cmake_minimum_required(VERSION 3.5...3.29)

project(ParquetExtension)

include_directories(
  include  ../../third_party/parquet
  ../../third_party/thrift)

add_subdirectory(decoder)
add_subdirectory(reader)
add_subdirectory(writer)
set(PARQUET_EXTENSION_FILES
    ${PARQUET_EXTENSION_FILES}
    column_reader.cc
    column_writer.cc
    parquet_crypto.cc
    parquet_extension.cc
    parquet_file_metadata_cache.cc
    parquet_float16.cc
    parquet_multi_file_info.cc
    parquet_metadata.cc
    parquet_reader.cc
    parquet_field_id.cc
    parquet_statistics.cc
    parquet_timestamp.cc
    parquet_writer.cc
    parquet_shredding.cc
    parquet_column_schema.cc
    parquet_geometry.cc
    serialize_parquet.cc
    zstd_file_system.cc)

if(NOT CLANG_TIDY)
  set(PARQUET_EXTENSION_FILES
      ${PARQUET_EXTENSION_FILES}
      ../../third_party/parquet/parquet_types.cc
      ../../third_party/thrift/thrift/protocol/TProtocol.cc
      ../../third_party/thrift/thrift/transport/TTransportException.cc
      ../../third_party/thrift/thrift/transport/TBufferTransports.cc
   )
endif()

build_static_extension(parquet ${PARQUET_EXTENSION_FILES})
set(PARAMETERS "-warnings")

build_loadable_extension(parquet ${PARAMETERS} ${PARQUET_EXTENSION_FILES})

install(
  TARGETS parquet_extension
  EXPORT "${GOOSE_EXPORT_SET}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")
