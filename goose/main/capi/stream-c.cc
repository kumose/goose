#include <goose/main/capi/capi_internal.h>
#include <goose/common/types/timestamp.h>
#include <goose/common/allocator.h>

goose_data_chunk goose_stream_fetch_chunk(goose_result result) {
	if (!result.internal_data) {
		return nullptr;
	}
	auto &result_data = *((goose::GooseResultData *)result.internal_data);
	if (result_data.result->type != goose::QueryResultType::STREAM_RESULT) {
		// We can only fetch from a StreamQueryResult
		return nullptr;
	}
	return goose_fetch_chunk(result);
}

goose_data_chunk goose_fetch_chunk(goose_result result) {
	if (!result.internal_data) {
		return nullptr;
	}
	auto &result_data = *((goose::GooseResultData *)result.internal_data);
	if (result_data.result_set_type == goose::CAPIResultSetType::CAPI_RESULT_TYPE_DEPRECATED) {
		return nullptr;
	}
	result_data.result_set_type = goose::CAPIResultSetType::CAPI_RESULT_TYPE_STREAMING;
	auto &result_instance = (goose::QueryResult &)*result_data.result;
	// FetchRaw ? Do we care about flattening them?
	try {
		auto chunk = result_instance.Fetch();
		return reinterpret_cast<goose_data_chunk>(chunk.release());
	} catch (std::exception &e) {
		return nullptr;
	}
}
