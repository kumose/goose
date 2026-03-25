#include <goose/execution/operator/helper/physical_pragma.h>

namespace goose {

SourceResultType PhysicalPragma::GetDataInternal(ExecutionContext &context, DataChunk &chunk,
                                                 OperatorSourceInput &input) const {
	auto &client = context.client;
	FunctionParameters parameters {info->parameters, info->named_parameters};
	info->function.function(client, parameters);

	return SourceResultType::FINISHED;
}

} // namespace goose
