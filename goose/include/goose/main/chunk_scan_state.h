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

#pragma once

#include <goose/common/vector.h>
#include <goose/common/unique_ptr.h>
#include <goose/common/error_data.h>

namespace goose {

class DataChunk;

//! Abstract chunk fetcher
class ChunkScanState {
public:
	explicit ChunkScanState();
	virtual ~ChunkScanState();

public:
	ChunkScanState(const ChunkScanState &other) = delete;
	ChunkScanState(ChunkScanState &&other) = default;
	ChunkScanState &operator=(const ChunkScanState &other) = delete;
	ChunkScanState &operator=(ChunkScanState &&other) = default;

public:
	virtual bool LoadNextChunk(ErrorData &error) = 0;
	virtual bool HasError() const = 0;
	virtual ErrorData &GetError() = 0;
	virtual const vector<LogicalType> &Types() const = 0;
	virtual const vector<string> &Names() const = 0;
	idx_t CurrentOffset() const;
	idx_t RemainingInChunk() const;
	DataChunk &CurrentChunk();
	bool ChunkIsEmpty() const;
	bool Finished() const;
	bool ScanStarted() const;
	void IncreaseOffset(idx_t increment, bool unsafe = false);

protected:
	idx_t offset = 0;
	bool finished = false;
	unique_ptr<DataChunk> current_chunk;
};

} // namespace goose
