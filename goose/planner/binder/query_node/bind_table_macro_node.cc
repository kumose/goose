#include <goose/catalog/catalog_entry/scalar_macro_catalog_entry.h>
#include <goose/parser/expression/function_expression.h>
#include <goose/parser/expression/subquery_expression.h>
#include <goose/parser/parsed_expression_iterator.h>
#include <goose/planner/expression_binder.h>
#include <goose/common/string_util.h>
#include <goose/common/limits.h>
#include <goose/main/config.h>
#include <goose/parser/expression/columnref_expression.h>
#include <goose/parser/expression/comparison_expression.h>
#include <goose/parser/query_node/select_node.h>
#include <goose/parser/tableref/joinref.h>
#include <goose/catalog/catalog_entry/table_macro_catalog_entry.h>
#include <goose/planner/binder.h>
#include <goose/function/table_macro_function.h>

namespace goose {

unique_ptr<QueryNode> Binder::BindTableMacro(FunctionExpression &function, TableMacroCatalogEntry &macro_func,
                                             idx_t depth) {
	// validate the arguments and separate positional and default arguments
	vector<unique_ptr<ParsedExpression>> positional_arguments;
	InsertionOrderPreservingMap<unique_ptr<ParsedExpression>> named_arguments;

	auto bind_result = MacroFunction::BindMacroFunction(*this, macro_func.macros, macro_func.name, function,
	                                                    positional_arguments, named_arguments, depth);
	if (!bind_result.error.empty()) {
		throw BinderException(function, bind_result.error);
	}
	auto &macro_def = *macro_func.macros[bind_result.function_idx.GetIndex()];

	auto new_macro_binding =
	    MacroFunction::CreateDummyBinding(macro_def, macro_func.name, positional_arguments, named_arguments);
	new_macro_binding->arguments = &positional_arguments;

	// We need an ExpressionBinder so that we can call ExpressionBinder::ReplaceMacroParametersRecursive()
	auto eb = ExpressionBinder(*this, this->context);

	eb.macro_binding = new_macro_binding.get();
	vector<unordered_set<string>> lambda_params;

	auto node = macro_def.Cast<TableMacroFunction>().query_node->Copy();
	ParsedExpressionIterator::EnumerateQueryNodeChildren(
	    *node, [&](unique_ptr<ParsedExpression> &child) { eb.ReplaceMacroParameters(child, lambda_params); });

	return node;
}

} // namespace goose
