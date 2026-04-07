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

#include <goose/extension/json/json_reader.h>
#include <goose/common/multi_file/multi_file_reader.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/types/type_map.h>
#include <goose/function/scalar/strftime_format.h>
#include <goose/function/table_function.h>
#include <goose/extension/json/json_enums.h>
#include <goose/extension/json/json_transform.h>
#include <goose/extension/json/json_reader_options.h>

namespace goose {

struct JSONScanData : public TableFunctionData {
public:
	JSONScanData();

	void InitializeFormats();
	void InitializeFormats(bool auto_detect);

public:
	//! JSON reader options
	JSONReaderOptions options;

	//! The set of keys to extract (case sensitive)
	vector<string> key_names;

	//! The date format map
	unique_ptr<DateFormatMap> date_format_map;
	//! Options when transforming the JSON to columnar data
	JSONTransformOptions transform_options;

	optional_idx max_threads;
	optional_idx estimated_cardinality_per_file;
};

struct JSONScanInfo : public TableFunctionInfo {
public:
	explicit JSONScanInfo(JSONScanType type_p = JSONScanType::INVALID, JSONFormat format_p = JSONFormat::AUTO_DETECT,
	                      JSONRecordType record_type_p = JSONRecordType::AUTO_DETECT, bool auto_detect_p = false)
	    : type(type_p), format(format_p), record_type(record_type_p), auto_detect(auto_detect_p) {
	}

	JSONScanType type;
	JSONFormat format;
	JSONRecordType record_type;
	bool auto_detect;
};

struct JSONScanGlobalState {
public:
	JSONScanGlobalState(ClientContext &context, const MultiFileBindData &bind_data);

public:
	//! Bound data
	const MultiFileBindData &bind_data;
	const JSONScanData &json_data;
	//! Options when transforming the JSON to columnar data
	JSONTransformOptions transform_options;

	//! Column names that we're actually reading (after projection pushdown)
	vector<string> names;
	vector<column_t> column_ids;
	vector<ColumnIndex> column_indices;

	//! Buffer manager allocator
	Allocator &allocator;
	//! The current buffer capacity
	idx_t buffer_capacity;

	//! Current number of threads active
	idx_t system_threads;
	//! Whether we enable parallel scans (only if less files than threads)
	bool enable_parallel_scans;

	bool file_is_assigned = false;
	bool initialized = false;
};

struct JSONScanLocalState {
public:
	JSONScanLocalState(ClientContext &context, JSONScanGlobalState &gstate);

public:
	idx_t Read();
	void AddTransformError(idx_t object_index, const string &error_message);

	JSONReaderScanState &GetScanState() {
		return scan_state;
	}

	const JSONReaderScanState &GetScanState() const {
		return scan_state;
	}

	bool TryInitializeScan(JSONScanGlobalState &gstate, JSONReader &reader);

public:
	//! Options when transforming the JSON to columnar data
	JSONTransformOptions transform_options;

private:
	void ParseJSON(char *const json_start, const idx_t json_size, const idx_t remaining);

private:
	//! Scan state
	JSONReaderScanState scan_state;
};

struct JSONGlobalTableFunctionState : public GlobalTableFunctionState {
public:
	JSONGlobalTableFunctionState(ClientContext &context, const MultiFileBindData &bind_data);

public:
	JSONScanGlobalState state;
};

struct JSONLocalTableFunctionState : public LocalTableFunctionState {
public:
	JSONLocalTableFunctionState(ClientContext &context, JSONScanGlobalState &gstate);

public:
	JSONScanLocalState state;
};

struct JSONScan {
public:
	static void AutoDetect(ClientContext &context, MultiFileBindData &bind_data, vector<LogicalType> &return_types,
	                       vector<string> &names);

	static void Serialize(Serializer &serializer, const optional_ptr<FunctionData> bind_data,
	                      const TableFunction &function);
	static unique_ptr<FunctionData> Deserialize(Deserializer &deserializer, TableFunction &function);

	static void TableFunctionDefaults(TableFunction &table_function);
};

} // namespace goose
