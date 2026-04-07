#pragma once

#include <goose/common/string.h>
#include <goose/common/types/value.h>
#include <goose/parser/constraint.h>

namespace goose {

struct ColumnConstraint {
	vector<unique_ptr<Constraint>> constraints;
	unique_ptr<ParsedExpression> default_value;
};

} // namespace goose
