#pragma once
#include <goose/parser/statement/select_statement.h>

namespace goose {
struct InsertValues {
	bool default_values = false;
	unique_ptr<SelectStatement> select_statement;
};
} // namespace goose
