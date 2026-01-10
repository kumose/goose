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
#include <goose/execution/operator/csv_scanner/scanner_boundary.h>
#include <goose/execution/operator/csv_scanner/csv_state_machine.h>
#include <goose/execution/operator/csv_scanner/csv_error.h>
#include <goose/function/table/read_csv.h>
#include <goose/execution/operator/csv_scanner/csv_file_scanner.h>
#include <goose/execution/operator/csv_scanner/string_value_scanner.h>
#include <goose/execution/operator/csv_scanner/csv_validator.h>

namespace goose {
struct MultiFileBindData;

//! CSV Global State is used in the CSV Reader Table Function, it controls what each thread
struct CSVGlobalState : public GlobalTableFunctionState {
public:
	CSVGlobalState(ClientContext &context_p, const CSVReaderOptions &options, idx_t total_file_count,
	               const MultiFileBindData &bind_data);

	~CSVGlobalState() override {
	}

	//! Generates a CSV Scanner, with information regarding the piece of buffer it should be read.
	//! In case it returns a nullptr it means we are done reading these files.
	void FinishScan(unique_ptr<StringValueScanner> scanner);
	unique_ptr<StringValueScanner> Next(shared_ptr<CSVFileScan> &file);
	void FinishLaunchingTasks(CSVFileScan &scan);

	void FillRejectsTable(CSVFileScan &scan);
	void FinishTask(CSVFileScan &scan);
	void FinishFile(CSVFileScan &scan);

	//! Whether or not to read individual CSV files single-threaded
	bool SingleThreadedRead() const {
		return single_threaded;
	}

private:
	//! Reference to the client context that created this scan
	ClientContext &context;
	const MultiFileBindData &bind_data;

	string sniffer_mismatch_error;

	bool initialized = false;

	bool single_threaded = false;

	atomic<idx_t> scanner_idx;

	shared_ptr<CSVBufferUsage> current_buffer_in_use;

	//! We hold information on the current scanner boundary
	CSVIterator current_boundary;

	vector<idx_t> rejects_file_indexes;
};

} // namespace goose
