#include <goose/function/table/system_functions.h>
#include <goose/parser/parsed_data/create_view_info.h>
#include <goose/parser/query_node/select_node.h>
#include <goose/parser/expression/star_expression.h>
#include <goose/parser/tableref/table_function_ref.h>
#include <goose/parser/expression/function_expression.h>
#include <goose/catalog/catalog.h>

namespace goose {

void BuiltinFunctions::RegisterSQLiteFunctions() {
	PragmaVersion::RegisterFunction(*this);
	PragmaPlatform::RegisterFunction(*this);
	PragmaCollations::RegisterFunction(*this);
	PragmaTableInfo::RegisterFunction(*this);
	PragmaStorageInfo::RegisterFunction(*this);
	PragmaMetadataInfo::RegisterFunction(*this);
	PragmaDatabaseSize::RegisterFunction(*this);
	PragmaUserAgent::RegisterFunction(*this);

	GooseConnectionCountFun::RegisterFunction(*this);
	GooseApproxDatabaseCountFun::RegisterFunction(*this);
	GooseColumnsFun::RegisterFunction(*this);
	GooseConstraintsFun::RegisterFunction(*this);
	GooseDatabasesFun::RegisterFunction(*this);
	GooseFunctionsFun::RegisterFunction(*this);
	GooseKeywordsFun::RegisterFunction(*this);
	GoosePreparedStatementsFun::RegisterFunction(*this);
	GooseLogFun::RegisterFunction(*this);
	GooseLogContextFun::RegisterFunction(*this);
	GooseIndexesFun::RegisterFunction(*this);
	GooseSchemasFun::RegisterFunction(*this);
	GooseDependenciesFun::RegisterFunction(*this);
	GooseExtensionsFun::RegisterFunction(*this);
	GooseMemoryFun::RegisterFunction(*this);
	GooseExternalFileCacheFun::RegisterFunction(*this);
	GooseOptimizersFun::RegisterFunction(*this);
	GooseSecretsFun::RegisterFunction(*this);
	GooseWhichSecretFun::RegisterFunction(*this);
	GooseSecretTypesFun::RegisterFunction(*this);
	GooseSequencesFun::RegisterFunction(*this);
	GooseSettingsFun::RegisterFunction(*this);
	GooseTablesFun::RegisterFunction(*this);
	GooseTableSample::RegisterFunction(*this);
	GooseTemporaryFilesFun::RegisterFunction(*this);
	GooseTypesFun::RegisterFunction(*this);
	GooseVariablesFun::RegisterFunction(*this);
	GooseViewsFun::RegisterFunction(*this);
	EnableLoggingFun::RegisterFunction(*this);
	EnableProfilingFun::RegisterFunction(*this);
	TestAllTypesFun::RegisterFunction(*this);
	TestVectorTypesFun::RegisterFunction(*this);
}

} // namespace goose
