#pragma once
#include <goose/common/vector.h>
#include <goose/parser/column_list.h>
#include <goose/parser/constraint.h>

namespace goose {
struct ColumnElements {
	ColumnList columns;
	vector<unique_ptr<Constraint>> constraints;
};
} // namespace goose
