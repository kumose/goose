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

#include <goose/execution/operator/csv_scanner/csv_buffer_manager.h>
#include <goose/execution/operator/csv_scanner/csv_state_machine.h>
#include <goose/execution/operator/csv_scanner/scanner_boundary.h>
#include <goose/execution/operator/csv_scanner/string_value_scanner.h>
#include <goose/execution/operator/csv_scanner/base_scanner.h>

namespace goose {

class SkipResult : public ScannerResult {
public:
	SkipResult(CSVStates &states, CSVStateMachine &state_machine, idx_t rows_to_skip);

	idx_t row_count = 0;
	idx_t rows_to_skip;

	//! Adds a Value to the result
	static inline void AddValue(SkipResult &result, const idx_t buffer_pos);
	//! Adds a Row to the result
	static inline bool AddRow(SkipResult &result, const idx_t buffer_pos);
	//! Behavior when hitting an invalid state
	static inline void InvalidState(SkipResult &result);
	//! Handles EmptyLine states
	static inline bool EmptyLine(SkipResult &result, const idx_t buffer_pos);
	//! Handles QuotedNewline State
	static inline void QuotedNewLine(SkipResult &result);
	//! Code to set comment
	static inline void SetComment(SkipResult &result, idx_t buffer_pos);
	//! Code to unset comment
	static inline bool UnsetComment(SkipResult &result, idx_t buffer_pos);
	//! Internal code to add a row
	inline void InternalAddRow();
};

//! Scanner used to skip lines in a CSV File
class SkipScanner : public BaseScanner {
public:
	SkipScanner(shared_ptr<CSVBufferManager> buffer_manager, const shared_ptr<CSVStateMachine> &state_machine,
	            shared_ptr<CSVErrorHandler> error_handler, idx_t rows_to_skip);

	SkipResult &ParseChunk() override;

	SkipResult &GetResult() override;

private:
	void Initialize() override;

	void FinalizeChunkProcess() override;

	SkipResult result;
};

} // namespace goose
