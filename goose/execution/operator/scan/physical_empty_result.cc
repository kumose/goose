#include <goose/execution/operator/scan/physical_empty_result.h>

namespace goose {

SourceResultType PhysicalEmptyResult::GetDataInternal(ExecutionContext &context, DataChunk &chunk,
                                                      OperatorSourceInput &input) const {
	return SourceResultType::FINISHED;
}

} // namespace goose
