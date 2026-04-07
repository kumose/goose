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
