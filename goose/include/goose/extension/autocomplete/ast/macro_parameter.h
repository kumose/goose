#pragma once
#include <goose/parser/parsed_expression.h>

namespace goose {
struct MacroParameter {
	unique_ptr<ParsedExpression> expression;
	string name;
	LogicalType type = LogicalType::UNKNOWN;
	bool is_default = false;
};

} // namespace goose
