#pragma once
#include <goose/parser/parsed_expression.h>

namespace goose {
struct JoinQualifier {
	unique_ptr<ParsedExpression> on_clause;
	vector<string> using_columns;
};

} // namespace goose
