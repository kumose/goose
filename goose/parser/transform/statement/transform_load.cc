#include <goose/parser/statement/load_statement.h>
#include <goose/parser/transformer.h>

namespace goose {

unique_ptr<LoadStatement> Transformer::TransformLoad(cantor::PGLoadStmt &stmt) {
	D_ASSERT(stmt.type == cantor::T_PGLoadStmt);

	auto load_stmt = make_uniq<LoadStatement>();
	auto load_info = make_uniq<LoadInfo>();
	load_info->filename = stmt.filename ? string(stmt.filename) : "";
	load_info->repository = stmt.repository ? string(stmt.repository) : "";
	load_info->repo_is_alias = stmt.repo_is_alias;
	load_info->version = stmt.version ? string(stmt.version) : "";
	switch (stmt.load_type) {
	case cantor::PG_LOAD_TYPE_LOAD:
		load_info->load_type = LoadType::LOAD;
		break;
	case cantor::PG_LOAD_TYPE_INSTALL:
		load_info->load_type = LoadType::INSTALL;
		break;
	case cantor::PG_LOAD_TYPE_FORCE_INSTALL:
		load_info->load_type = LoadType::FORCE_INSTALL;
		break;
	}
	load_stmt->info = std::move(load_info);
	return load_stmt;
}

} // namespace goose
