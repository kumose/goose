// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

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
