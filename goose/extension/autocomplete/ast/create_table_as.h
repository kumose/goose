#pragma once
#include <goose/parser/statement/select_statement.h>

namespace goose {
struct CreateTableAs {
	bool with_data = false;
	unique_ptr<SelectStatement> select_statement;
	ColumnList column_names;
};

} // namespace goose
