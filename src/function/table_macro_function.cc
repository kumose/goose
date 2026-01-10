//===----------------------------------------------------------------------===//
//                         Goose
//
// goose/function/table_macro_function.hpp
//
//
//===----------------------------------------------------------------------===//
//! The SelectStatement of the view
#include <goose/function/table_macro_function.h>

#include <goose/parser/expression/constant_expression.h>
#include <goose/parser/query_node.h>

namespace goose {

TableMacroFunction::TableMacroFunction(unique_ptr<QueryNode> query_node)
    : MacroFunction(MacroType::TABLE_MACRO), query_node(std::move(query_node)) {
}

TableMacroFunction::TableMacroFunction(void) : MacroFunction(MacroType::TABLE_MACRO) {
}

unique_ptr<MacroFunction> TableMacroFunction::Copy() const {
	auto result = make_uniq<TableMacroFunction>();
	result->query_node = query_node->Copy();
	this->CopyProperties(*result);
	return std::move(result);
}

string TableMacroFunction::ToSQL() const {
	return MacroFunction::ToSQL() + StringUtil::Format("TABLE (%s)", query_node->ToString());
}

} // namespace goose
