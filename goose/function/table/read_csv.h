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

#include <goose/common/multi_file/multi_file_reader.h>
#include <goose/execution/operator/csv_scanner/csv_buffer.h>
#include <goose/execution/operator/csv_scanner/csv_buffer_manager.h>
#include <goose/execution/operator/csv_scanner/csv_file_handle.h>
#include <goose/execution/operator/csv_scanner/csv_reader_options.h>
#include <goose/execution/operator/csv_scanner/csv_state_machine_cache.h>
#include <goose/function/built_in_functions.h>
#include <goose/function/scalar/strftime_format.h>
#include <goose/function/table_function.h>
#include <goose/execution/operator/csv_scanner/csv_file_scanner.h>
#include <goose/common/csv_writer.h>

namespace goose {
class BaseScanner;
class StringValueScanner;

class ReadCSV {
public:
	static unique_ptr<CSVFileHandle> OpenCSV(const OpenFileInfo &file, const CSVReaderOptions &options,
	                                         ClientContext &context);
};

struct BaseCSVData : public TableFunctionData {
	//! The CSV reader options
	CSVReaderOptions options;
	//! Offsets for generated columns
	idx_t filename_col_idx {};
	idx_t hive_partition_col_idx {};

	void Finalize();
};

struct WriteCSVData : public BaseCSVData {
	explicit WriteCSVData(vector<string> names) {
		options.name_list = std::move(names);
		if (options.dialect_options.state_machine_options.escape == '\0') {
			options.dialect_options.state_machine_options.escape = options.dialect_options.state_machine_options.quote;
		}
	}
	//! The size of the CSV file (in bytes) that we buffer before we flush it to disk
	idx_t flush_size = 4096ULL * 8ULL;
	//! Expressions used to convert the input into strings
	vector<unique_ptr<Expression>> cast_expressions;
};

struct ColumnInfo {
	ColumnInfo() {
	}
	ColumnInfo(vector<std::string> names_p, vector<LogicalType> types_p) {
		names = std::move(names_p);
		types = std::move(types_p);
	}
	void Serialize(Serializer &serializer) const;
	static ColumnInfo Deserialize(Deserializer &deserializer);

	vector<std::string> names;
	vector<LogicalType> types;
};

struct ReadCSVData : public BaseCSVData {
	ReadCSVData();
	//! If the sql types from the file were manually set
	vector<bool> manually_set;
	//! The buffer manager (if any): this is used when automatic detection is used during binding.
	//! In this case, some CSV buffers have already been read and can be reused.
	shared_ptr<CSVBufferManager> buffer_manager;
	//! Column info (used for union reader serialization)
	vector<ColumnInfo> column_info;
	//! The CSV schema, in case there is a unified schema that all files must read
	CSVSchema csv_schema;

	void FinalizeRead(ClientContext &context);
};

struct SerializedCSVReaderOptions {
	SerializedCSVReaderOptions() = default;
	SerializedCSVReaderOptions(CSVReaderOptions options, MultiFileOptions file_options);
	SerializedCSVReaderOptions(CSVOption<char> single_byte_delimiter, const CSVOption<string> &multi_byte_delimiter);

	CSVReaderOptions options;
	MultiFileOptions file_options;

	void Serialize(Serializer &serializer) const;
	static SerializedCSVReaderOptions Deserialize(Deserializer &deserializer);
};

struct SerializedReadCSVData {
	vector<string> files;
	vector<LogicalType> csv_types;
	vector<string> csv_names;
	vector<LogicalType> return_types;
	vector<string> return_names;
	idx_t filename_col_idx;
	SerializedCSVReaderOptions options;
	MultiFileReaderBindData reader_bind;
	vector<ColumnInfo> column_info;

	void Serialize(Serializer &serializer) const;
	static SerializedReadCSVData Deserialize(Deserializer &deserializer);
};

struct CSVCopyFunction {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct ReadCSVTableFunction {
	static TableFunction GetFunction();
	static TableFunction GetAutoFunction();
	static void ReadCSVAddNamedParameters(TableFunction &table_function);
	static void RegisterFunction(BuiltinFunctions &set);
};

} // namespace goose
