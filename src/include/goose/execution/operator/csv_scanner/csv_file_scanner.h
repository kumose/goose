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
#include <goose/execution/operator/csv_scanner/csv_schema.h>
#include <goose/execution/operator/csv_scanner/csv_validator.h>
#include <goose/common/multi_file/base_file_reader.h>

namespace goose {
struct ReadCSVData;
class CSVFileScan;

struct CSVUnionData : public BaseUnionData {
	explicit CSVUnionData(OpenFileInfo file_p) : BaseUnionData(std::move(file_p)) {
	}
	~CSVUnionData() override;

	CSVReaderOptions options;
};

//! Struct holding information over a CSV File we will scan
class CSVFileScan : public BaseFileReader {
public:
	//! Constructor for new CSV Files, we must initialize the buffer manager and the state machine
	//! Path to this file
	CSVFileScan(ClientContext &context, const OpenFileInfo &file, CSVReaderOptions options,
	            const MultiFileOptions &file_options, const vector<string> &names, const vector<LogicalType> &types,
	            CSVSchema &file_schema, bool per_file_single_threaded,
	            shared_ptr<CSVBufferManager> buffer_manager = nullptr, bool fixed_schema = false);

	CSVFileScan(ClientContext &context, const OpenFileInfo &file, const CSVReaderOptions &options,
	            const MultiFileOptions &file_options);

public:
	void SetStart();
	void SetNamesAndTypes(const vector<string> &names, const vector<LogicalType> &types);

public:
	string GetReaderType() const override {
		return "CSV";
	}

	bool UseCastMap() const override {
		//! Whether or not to push casts into the cast map
		return true;
	}

	shared_ptr<BaseUnionData> GetUnionData(idx_t file_idx) override;
	void PrepareReader(ClientContext &context, GlobalTableFunctionState &) override;
	bool TryInitializeScan(ClientContext &context, GlobalTableFunctionState &gstate,
	                       LocalTableFunctionState &lstate) override;
	AsyncResult Scan(ClientContext &context, GlobalTableFunctionState &global_state,
	                 LocalTableFunctionState &local_state, DataChunk &chunk) override;
	void FinishFile(ClientContext &context, GlobalTableFunctionState &gstate_p) override;
	double GetProgressInFile(ClientContext &context) override;

public:
	idx_t GetFileIndex() const {
		return file_list_idx.GetIndex();
	}
	const vector<string> &GetNames();
	const vector<LogicalType> &GetTypes();
	void InitializeProjection();
	void Finish();

	//! Initialize the actual names and types to be scanned from the file
	void InitializeFileNamesTypes();

public:
	//! Buffer Manager for the CSV File
	shared_ptr<CSVBufferManager> buffer_manager;
	//! State Machine for this file
	shared_ptr<CSVStateMachine> state_machine;
	//! How many bytes were read up to this point
	atomic<idx_t> bytes_read {0};
	//! Size of this file
	idx_t file_size;
	//! Line Info used in error messages
	shared_ptr<CSVErrorHandler> error_handler;
	//! Whether or not this is an on-disk file
	bool on_disk_file = true;

	vector<LogicalType> file_types;

	//! Variables to handle projection pushdown
	set<idx_t> projected_columns;
	std::vector<std::pair<idx_t, idx_t>> projection_ids;

	//! Options for this CSV Reader
	CSVReaderOptions options;

	CSVIterator start_iterator;

	CSVValidator validator;
	idx_t skipped_rows = 0;

	//! The started tasks and finished tasks allow us to track if all reads of the CSV file have completed
	//! Note that the "started_tasks" starts at one - this is so we can track when the scheduling of all tasks for this
	//! file has completed When the scheduling is finished we increment `finished_tasks` by one as well
	atomic<idx_t> started_tasks {1};
	atomic<idx_t> finished_tasks {0};

private:
	vector<string> names;
	vector<LogicalType> types;
};
} // namespace goose
