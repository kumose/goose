
add_subdirectory(constraints)
add_subdirectory(expression)
add_subdirectory(parsed_data)
add_subdirectory(query_node)
add_subdirectory(statement)
add_subdirectory(tableref)
add_subdirectory(transform)

add_library_unity(
  goose_parser
  OBJECT
  base_expression.cc
  column_definition.cc
  column_list.cc
  constraint.cc
  expression_util.cc
  keyword_helper.cc
  parsed_expression.cc
  parsed_expression_iterator.cc
  parser.cc
  qualified_name.cc
  query_error_context.cc
  query_node.cc
  result_modifier.cc
  tableref.cc
  transformer.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_parser>
    PARENT_SCOPE)
