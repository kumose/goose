#include <goose/execution/operator/helper/physical_prepare.h>
#include <goose/main/client_data.h>

namespace goose {

SourceResultType PhysicalPrepare::GetDataInternal(ExecutionContext &context, DataChunk &chunk,
                                                  OperatorSourceInput &input) const {
	auto &client = context.client;

	// store the prepared statement in the context
	ClientData::Get(client).prepared_statements[name.ToStdString()] = prepared;

	return SourceResultType::FINISHED;
}

} // namespace goose
