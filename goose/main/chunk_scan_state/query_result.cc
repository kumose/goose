#include <goose/main/query_result.h>
#include <goose/main/stream_query_result.h>
#include <goose/main/chunk_scan_state/query_result.h>

namespace goose {
    QueryResultChunkScanState::QueryResultChunkScanState(QueryResult &result) : ChunkScanState(), result(result) {
    }

    QueryResultChunkScanState::~QueryResultChunkScanState() {
    }

    bool QueryResultChunkScanState::InternalLoad(ErrorData &error) {
        D_ASSERT(!finished);
        if (result.type == QueryResultType::STREAM_RESULT) {
            auto &stream_result = result.Cast<StreamQueryResult>();
            if (!stream_result.IsOpen()) {
                return true;
            }
        }
        return result.TryFetch(current_chunk, error);
    }

    bool QueryResultChunkScanState::HasError() const {
        return result.HasError();
    }

    ErrorData &QueryResultChunkScanState::GetError() {
        D_ASSERT(result.HasError());
        return result.GetErrorObject();
    }

    const vector<LogicalType> &QueryResultChunkScanState::Types() const {
        return result.types;
    }

    const vector<string> &QueryResultChunkScanState::Names() const {
        return result.names;
    }

    bool QueryResultChunkScanState::LoadNextChunk(ErrorData &error) {
        if (finished) {
            return !finished;
        }
        auto load_result = InternalLoad(error);
        if (!load_result) {
            finished = true;
        }
        offset = 0;
        return !finished;
    }
} // namespace goose
