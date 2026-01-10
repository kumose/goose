#include <goose/parser/statement/create_statement.h>

namespace goose {

CreateStatement::CreateStatement() : SQLStatement(StatementType::CREATE_STATEMENT) {
}

CreateStatement::CreateStatement(const CreateStatement &other) : SQLStatement(other), info(other.info->Copy()) {
}

unique_ptr<SQLStatement> CreateStatement::Copy() const {
	return unique_ptr<CreateStatement>(new CreateStatement(*this));
}

string CreateStatement::ToString() const {
	return info->ToString();
}

} // namespace goose
