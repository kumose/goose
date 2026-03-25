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

#include <goose/storage/compression/patas/patas.h>
#include <goose/storage/compression/patas/patas_scan.h>

#include <goose/common/limits.h>
#include <goose/common/types/null_value.h>
#include <goose/function/compression/compression.h>
#include <goose/function/compression_function.h>
#include <goose/main/config.h>
#include <goose/storage/buffer_manager.h>

#include <goose/storage/table/column_data_checkpointer.h>
#include <goose/storage/table/column_segment.h>
#include <goose/common/operator/subtract.h>

namespace goose {

template <class T>
void PatasFetchRow(ColumnSegment &segment, ColumnFetchState &state, row_t row_id, Vector &result, idx_t result_idx) {
	using EXACT_TYPE = typename FloatingToExact<T>::TYPE;

	PatasScanState<T> scan_state(segment);
	scan_state.Skip(segment, UnsafeNumericCast<idx_t>(row_id));
	auto result_data = FlatVector::GetDataUnsafe<EXACT_TYPE>(result);
	result_data[result_idx] = (EXACT_TYPE)0;

	if (scan_state.GroupFinished() && scan_state.total_value_count < scan_state.count) {
		scan_state.LoadGroup(scan_state.group_state.values);
	}
	scan_state.group_state.Scan((uint8_t *)(result_data + result_idx), 1);
	scan_state.total_value_count++;
}

} // namespace goose
