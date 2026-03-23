add_library_unity(
  goose_common_tree_renderer
  OBJECT
  json_tree_renderer.cc
  html_tree_renderer.cc
  graphviz_tree_renderer.cc
  text_tree_renderer.cc
  yaml_tree_renderer.cc
  mermaid_tree_renderer.cc
  tree_renderer.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_common_tree_renderer>
    PARENT_SCOPE)
