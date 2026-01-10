#include <goose/parser/statement/load_statement.h>
#include <goose/planner/binder.h>
#include <goose/planner/operator/logical_simple.h>
#include <goose/main/extension_install_info.h>
#include <algorithm>

namespace goose {

BoundStatement Binder::Bind(LoadStatement &stmt) {
	BoundStatement result;
	result.types = {LogicalType::BOOLEAN};
	result.names = {"Success"};

	// Ensure the repository exists if it's an alias
	if (!stmt.info->repository.empty() && stmt.info->repo_is_alias) {
		auto repository_url = ExtensionRepository::TryGetRepositoryUrl(stmt.info->repository);
		if (repository_url.empty()) {
			throw BinderException("'%s' is not a known repository name. Are you trying to query from a repository by "
			                      "path? Use single quotes: `FROM '%s'`",
			                      stmt.info->repository, stmt.info->repository);
		}
	}

	result.plan = make_uniq<LogicalSimple>(LogicalOperatorType::LOGICAL_LOAD, std::move(stmt.info));

	auto &properties = GetStatementProperties();
	properties.output_type = QueryResultOutputType::FORCE_MATERIALIZED;
	properties.return_type = StatementReturnType::NOTHING;
	return result;
}

} // namespace goose
