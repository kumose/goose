#include <goose/function/function.h>
#include <goose/parser/expression/function_expression.h>
#include <goose/parser/statement/call_statement.h>
#include <goose/extension/autocomplete/transformer/peg_transformer.h>

namespace goose {

unique_ptr<SQLStatement> PEGTransformerFactory::TransformCallStatement(PEGTransformer &transformer,
                                                                       optional_ptr<ParseResult> parse_result) {
	auto &list_pr = parse_result->Cast<ListParseResult>();
	auto table_function_name = list_pr.Child<IdentifierParseResult>(1).identifier;
	auto function_children =
	    transformer.Transform<vector<unique_ptr<ParsedExpression>>>(list_pr.Child<ListParseResult>(2));
	auto result = make_uniq<CallStatement>();
	auto function_expression = make_uniq<FunctionExpression>(table_function_name, std::move(function_children));
	result->function = std::move(function_expression);
	return std::move(result);
}

} // namespace goose
