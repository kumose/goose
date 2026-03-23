#include <goose/parser/statement/drop_statement.h>

namespace goose {

DropStatement::DropStatement() : SQLStatement(StatementType::DROP_STATEMENT), info(make_uniq<DropInfo>()) {
}

DropStatement::DropStatement(const DropStatement &other) : SQLStatement(other), info(other.info->Copy()) {
}

unique_ptr<SQLStatement> DropStatement::Copy() const {
	return unique_ptr<DropStatement>(new DropStatement(*this));
}

string DropStatement::ToString() const {
	return info->ToString();
}

} // namespace goose
