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

#include <goose/common/types/row/tuple_data_states.h>
#include <goose/execution/expression_executor.h>

namespace goose {

class Sort;
class SortedRun;
class BufferManager;
class DataChunk;
class TupleDataCollection;
class TupleDataLayout;

class SortedRunScanState {
public:
	SortedRunScanState(ClientContext &context, const Sort &sort);

public:
	void Scan(const SortedRun &sorted_run, const Vector &sort_key_pointers, const idx_t &count, DataChunk &chunk);

private:
	template <SortKeyType sort_key_type>
	void TemplatedScan(const SortedRun &sorted_run, const Vector &sort_key_pointers, const idx_t &count,
	                   DataChunk &chunk);

private:
	const Sort &sort;
	ExpressionExecutor key_executor;
	DataChunk key;
	DataChunk decoded_key;
	TupleDataScanState payload_state;
	vector<char> key_buffer;
};

class SortedRun {
public:
	SortedRun(ClientContext &context, const Sort &sort, bool is_index_sort);
	unique_ptr<SortedRun> CreateRunForMaterialization() const;
	~SortedRun();

public:
	//! Appends data to key/data collections
	void Sink(DataChunk &key, DataChunk &payload);
	//! Sorts the data (physically reorder data if external)
	void Finalize(bool external);
	//! Destroy data between these tuple indices
	void DestroyData(idx_t tuple_idx_begin, idx_t tuple_idx_end);
	//! Number of tuples
	idx_t Count() const;
	//! Size of this sorted run
	idx_t SizeInBytes() const;

private:
	mutex merger_global_state_lock;
	unique_ptr<GlobalSourceState> merge_global_state;

public:
	ClientContext &context;
	const Sort &sort;

	//! Key and payload collections (and associated append states)
	unique_ptr<TupleDataCollection> key_data;
	unique_ptr<TupleDataCollection> payload_data;
	TupleDataAppendState key_append_state;
	TupleDataAppendState payload_append_state;

	//! Whether this is an (approximate) index sort
	const bool is_index_sort;

	//! Whether this run has been finalized
	bool finalized;
};

} // namespace goose
