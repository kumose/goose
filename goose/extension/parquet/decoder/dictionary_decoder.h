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

#include <goose/goose.h>
#include <goose/extension/parquet/parquet_rle_bp_decoder.h>
#include <goose/extension/parquet/resizable_buffer.h>

namespace goose {
class ColumnReader;
struct TableFilterState;

class DictionaryDecoder {
public:
	explicit DictionaryDecoder(ColumnReader &reader);

public:
	void InitializeDictionary(idx_t dictionary_size, optional_ptr<const TableFilter> filter,
	                          optional_ptr<TableFilterState> filter_state, bool has_defines);
	void InitializePage();
	idx_t Read(uint8_t *defines, idx_t read_count, Vector &result, idx_t result_offset);
	void Skip(uint8_t *defines, idx_t skip_count);
	bool CanFilter(const TableFilter &filter, TableFilterState &filter_state);
	bool DictionarySupportsFilter(const TableFilter &filter, TableFilterState &filter_state);
	void Filter(uint8_t *defines, idx_t read_count, Vector &result, SelectionVector &sel, idx_t &approved_tuple_count);
	bool HasFilter() const {
		return filter_result.get();
	}
	bool HasFilteredOutAllValues() const {
		return HasFilter() && filter_count == 0;
	}

private:
	idx_t GetValidValues(uint8_t *defines, idx_t read_count, idx_t result_offset);
	void ConvertDictToSelVec(uint32_t *offsets, const SelectionVector &rows, idx_t count);

private:
	ColumnReader &reader;
	ResizeableBuffer &offset_buffer;
	unique_ptr<RleBpDecoder> dict_decoder;
	SelectionVector valid_sel;
	SelectionVector dictionary_selection_vector;
	idx_t dictionary_size;
	buffer_ptr<VectorChildBuffer> dictionary;
	unsafe_unique_array<bool> filter_result;
	idx_t filter_count;
	bool can_have_nulls;
};

} // namespace goose
