#pragma once

#include <goose/common/types/value.h>
#include <goose/parser/column_definition.h>
#include <goose/parser/constraint.h>

namespace goose {

struct AddColumnEntry {
	LogicalType type;
	vector<string> column_path;
	unique_ptr<ParsedExpression> default_value;
};

} // namespace goose
