// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

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
