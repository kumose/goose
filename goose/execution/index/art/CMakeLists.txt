add_library_unity(
  goose_execution_index_art
  OBJECT
  art_builder.cc
  art.cc
  art_index.cc
  art_key.cc
  art_merger.cc
  base_leaf.cc
  base_node.cc
  iterator.cc
  leaf.cc
  node.cc
  node256.cc
  node256_leaf.cc
  node48.cc
  prefix.cc
  prefix_handle.cc
  const_prefix_handle.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_execution_index_art>
    PARENT_SCOPE)
