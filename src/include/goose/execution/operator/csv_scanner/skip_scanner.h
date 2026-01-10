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
