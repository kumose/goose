#include <goose/parser/statement/load_statement.h>

namespace goose {

LoadStatement::LoadStatement() : SQLStatement(StatementType::LOAD_STATEMENT) {
}

LoadStatement::LoadStatement(const LoadStatement &other) : SQLStatement(other), info(other.info->Copy()) {
}

unique_ptr<SQLStatement> LoadStatement::Copy() const {
	return unique_ptr<LoadStatement>(new LoadStatement(*this));
}

string LoadStatement::ToString() const {
	return info->ToString();
}

} // namespace goose
