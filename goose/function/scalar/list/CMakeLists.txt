add_library_unity(
  goose_func_list_nested
  OBJECT
  contains_or_position.cc
  list_extract.cc
  list_intersect.cc
  list_resize.cc
  list_zip.cc
  list_select.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_list_nested>
    PARENT_SCOPE)
