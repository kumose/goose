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

#include <goose/common/types/column/column_data_scan_states.h>

namespace goose {
class ColumnDataCollection;

class ColumnDataChunkIterationHelper {
public:
	GOOSE_API ColumnDataChunkIterationHelper(const ColumnDataCollection &collection, vector<column_t> column_ids);

private:
	const ColumnDataCollection &collection;
	vector<column_t> column_ids;

private:
	class ColumnDataChunkIterator;

	class ColumnDataChunkIterator {
	public:
		GOOSE_API explicit ColumnDataChunkIterator(const ColumnDataCollection *collection_p,
		                                            vector<column_t> column_ids);

		const ColumnDataCollection *collection;
		ColumnDataScanState scan_state;
		shared_ptr<DataChunk> scan_chunk;
		idx_t row_index;

	public:
		GOOSE_API void Next();

		GOOSE_API ColumnDataChunkIterator &operator++();
		GOOSE_API bool operator!=(const ColumnDataChunkIterator &other) const;
		GOOSE_API DataChunk &operator*() const;
	};

public:
	ColumnDataChunkIterator begin() { // NOLINT: match stl API
		return ColumnDataChunkIterator(&collection, column_ids);
	}
	ColumnDataChunkIterator end() { // NOLINT: match stl API
		return ColumnDataChunkIterator(nullptr, vector<column_t>());
	}
};

class ColumnDataRowIterationHelper {
public:
	GOOSE_API explicit ColumnDataRowIterationHelper(const ColumnDataCollection &collection);

private:
	const ColumnDataCollection &collection;

private:
	class ColumnDataRowIterator;

	class ColumnDataRowIterator {
	public:
		GOOSE_API explicit ColumnDataRowIterator(
		    const ColumnDataCollection *collection_p,
		    ColumnDataScanProperties properties = ColumnDataScanProperties::DISALLOW_ZERO_COPY);

		const ColumnDataCollection *collection;
		ColumnDataScanState scan_state;
		shared_ptr<DataChunk> scan_chunk;
		ColumnDataRow current_row;

	public:
		void Next();

		GOOSE_API ColumnDataRowIterator &operator++();
		GOOSE_API bool operator!=(const ColumnDataRowIterator &other) const;
		GOOSE_API const ColumnDataRow &operator*() const;
	};

public:
	GOOSE_API ColumnDataRowIterator begin(); // NOLINT: match stl API
	GOOSE_API ColumnDataRowIterator end();   // NOLINT: match stl API
};

} // namespace goose
