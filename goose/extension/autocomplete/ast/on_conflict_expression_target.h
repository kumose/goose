#pragma once

#include <goose/common/enums/set_scope.h>
#include <goose/common/string.h>
#include <goose/common/vector.h>

namespace goose {

struct OnConflictExpressionTarget {
	vector<string> indexed_columns;
	unique_ptr<ParsedExpression> where_clause; // Default value is defined here
};

} // namespace goose
