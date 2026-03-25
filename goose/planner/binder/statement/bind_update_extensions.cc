#include <goose/parser/statement/load_statement.h>
#include <goose/planner/binder.h>
#include <goose/parser/statement/update_extensions_statement.h>
#include <goose/planner/operator/logical_simple.h>
#include <algorithm>

namespace goose {

BoundStatement Binder::Bind(UpdateExtensionsStatement &stmt) {
	BoundStatement result;

	result.names.emplace_back("extension_name");
	result.types.emplace_back(LogicalType::VARCHAR);
	result.names.emplace_back("repository");
	result.types.emplace_back(LogicalType::VARCHAR);
	result.names.emplace_back("update_result");
	result.types.emplace_back(LogicalType::VARCHAR);
	result.names.emplace_back("previous_version");
	result.types.emplace_back(LogicalType::VARCHAR);
	result.names.emplace_back("current_version");
	result.types.emplace_back(LogicalType::VARCHAR);

	result.plan = make_uniq<LogicalSimple>(LogicalOperatorType::LOGICAL_UPDATE_EXTENSIONS, std::move(stmt.info));

	return result;
}

} // namespace goose
