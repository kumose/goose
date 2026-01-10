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
#include <goose/execution/operator/csv_scanner/csv_file_handle.h>
#include <goose/execution/operator/csv_scanner/csv_reader_options.h>
#include <goose/execution/operator/csv_scanner/csv_state_machine.h>
#include <goose/main/client_context.h>
#include <goose/common/multi_file/multi_file_reader.h>
#include <goose/common/fast_mem.h>

//! We all need boundaries every now and then, CSV Scans also need them
//! This class keeps track of what a scan should read, so which buffer and from where to where
//! As in real life, much like my childhood country, the rules are not really enforced.
//! So the end boundaries of a Scanner Boundary can and will be pushed.
//! In practice this means that a scanner is tolerated to read one line over it's end.
namespace goose {

//! Information stored in the buffer
struct CSVBoundary {
	CSVBoundary(idx_t buffer_idx, idx_t buffer_pos, idx_t boundary_idx, idx_t end_pos);
	CSVBoundary();
	void Print() const;
	//! Start Buffer index of the file where we start scanning
	idx_t buffer_idx = 0;
	//! Start Buffer position of the buffer of the file where we start scanning
	//! This position moves as we move through the buffer
	idx_t buffer_pos = 0;
	//! The boundary index relative to the total scan, only used for parallel reading to enforce
	//! Insertion Order
	idx_t boundary_idx = 0;
	//! Last position this iterator should read.
	idx_t end_pos;
};

struct CSVPosition {
	CSVPosition(idx_t buffer_idx, idx_t buffer_pos);
	CSVPosition();
	//! Start Buffer index of the file where we start scanning
	idx_t buffer_idx = 0;
	//! Start Buffer position of the buffer of the file where we start scanning
	//! This position moves as we move through the buffer
	idx_t buffer_pos = 0;
};
struct CSVIterator {
public:
	CSVIterator();

	void Print() const;
	//! Moves the boundary to the next one to be scanned, if there are no next boundaries, it returns False
	//! Otherwise, if there are boundaries, it returns True
	bool Next(CSVBufferManager &buffer_manager, const CSVReaderOptions &reader_options);
	//! If boundary is set
	bool IsBoundarySet() const;

	//! Getters
	idx_t GetEndPos() const;
	idx_t GetBufferIdx() const;
	idx_t GetBoundaryIdx() const;

	void SetCurrentPositionToBoundary();

	void SetCurrentBoundaryToPosition(bool single_threaded, const CSVReaderOptions &reader_options);

	void SetStart(idx_t pos);
	void SetEnd(idx_t pos);

	// Gets the current position for the file
	idx_t GetGlobalCurrentPos() const;

	//! Checks if we are done with this iterator
	void CheckIfDone();

	static idx_t BytesPerThread(const CSVReaderOptions &reader_options);

	static constexpr idx_t ROWS_PER_THREAD = 4;

	CSVPosition pos;

	bool done = false;

	bool first_one = true;

	idx_t buffer_size;

private:
	//! The original setting
	CSVBoundary boundary;
	//! Sometimes life knows no boundaries.
	//! The boundaries don't have to be set for single-threaded execution.
	bool is_set;
};
} // namespace goose
