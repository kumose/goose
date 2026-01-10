add_library_unity(
        goose_common_arrow_appender
  OBJECT
  bool_data.cc
  fixed_size_list_data.cc
  null_data.cc
  struct_data.cc
  union_data.cc
  append_data.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_common_arrow_appender>
    PARENT_SCOPE)
