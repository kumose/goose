#pragma once

#include <goose/main/chunk_scan_state.h>
#include <goose/common/error_data.h>

namespace goose {

class QueryResult;

class QueryResultChunkScanState : public ChunkScanState {
public:
	explicit QueryResultChunkScanState(QueryResult &result);
	~QueryResultChunkScanState() override;

public:
	bool LoadNextChunk(ErrorData &error) override;
	bool HasError() const override;
	ErrorData &GetError() override;
	const vector<LogicalType> &Types() const override;
	const vector<string> &Names() const override;

private:
	bool InternalLoad(ErrorData &error);

private:
	QueryResult &result;
};

} // namespace goose
