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

#include <goose/goose.h>
#include "parquet_rle_bp_decoder.h"
#include "resizable_buffer.h"

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
