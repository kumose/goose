cmake_minimum_required(VERSION 2.8.12...3.29)

project(JSONExtension)

include_directories(include)
add_subdirectory(json_functions)

set(JSON_EXTENSION_FILES
    json_reader.cc
    json_extension.cc
    json_common.cc
    json_enums.cc
    json_functions.cc
    json_multi_file_info.cc
    json_scan.cc
    json_serializer.cc
    json_deserializer.cc
    serialize_json.cc
    json_functions/copy_json.cc
    json_functions/json_array_length.cc
    json_functions/json_contains.cc
    json_functions/json_exists.cc
    json_functions/json_extract.cc
    json_functions/json_keys.cc
    json_functions/json_merge_patch.cc
    json_functions/json_pretty.cc
    json_functions/json_table_in_out.cc
    json_functions/json_structure.cc
    json_functions/json_transform.cc
    json_functions/json_create.cc
    json_functions/json_type.cc
    json_functions/json_valid.cc
    json_functions/json_value.cc
    json_functions/json_serialize_sql.cc
    json_functions/json_serialize_plan.cc
    json_functions/read_json.cc
    json_functions/read_json_objects.cc)

build_static_extension(json ${JSON_EXTENSION_FILES})
set(PARAMETERS "-warnings")
build_loadable_extension(json ${PARAMETERS} ${JSON_EXTENSION_FILES})

install(
  TARGETS json_extension
  EXPORT "${GOOSE_EXPORT_SET}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")
