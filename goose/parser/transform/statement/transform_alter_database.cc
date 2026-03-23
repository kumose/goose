#include <goose/parser/statement/alter_statement.h>
#include <goose/parser/parsed_data/alter_database_info.h>
#include <goose/parser/transformer.h>

namespace goose {

unique_ptr<AlterStatement> Transformer::TransformAlterDatabase(cantor::PGAlterDatabaseStmt &stmt) {
	auto result = make_uniq<AlterStatement>();

	auto database_name = stmt.dbname ? string(stmt.dbname) : string();
	if (database_name.empty()) {
		throw ParserException("ALTER DATABASE requires a database name");
	}

	OnEntryNotFound if_not_found = OnEntryNotFound::THROW_EXCEPTION;
	if (stmt.missing_ok) {
		if_not_found = OnEntryNotFound::RETURN_NULL;
	}

	switch (stmt.alter_type) {
	case cantor::PG_ALTER_DATABASE_RENAME: {
		if (!stmt.new_name) {
			throw ParserException("ALTER DATABASE RENAME requires a new name");
		}
		auto info = make_uniq<RenameDatabaseInfo>(database_name, string(stmt.new_name), if_not_found);
		result->info = std::move(info);
		break;
	}
	default:
		throw ParserException("Unsupported ALTER DATABASE operation");
	}

	return result;
}

} // namespace goose
