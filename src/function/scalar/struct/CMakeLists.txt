add_library_unity(
  goose_func_struct_main
  OBJECT
  remap_struct.cc
  struct_extract.cc
  struct_pack.cc
  struct_concat.cc
  struct_contains.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_struct_main>
    PARENT_SCOPE)
