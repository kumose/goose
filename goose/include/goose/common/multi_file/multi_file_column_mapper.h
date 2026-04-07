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

namespace goose {
struct ResultColumnMapping;
struct ColumnMapper;

class MultiFileColumnMapper {
public:
	MultiFileColumnMapper(ClientContext &context, MultiFileReader &multi_file_reader, MultiFileReaderData &reader_data,
	                      const vector<MultiFileColumnDefinition> &global_columns,
	                      const vector<ColumnIndex> &global_column_ids, optional_ptr<TableFilterSet> filters,
	                      MultiFileList &multi_file_list, const virtual_column_map_t &virtual_columns);

public:
	ReaderInitializeType CreateMapping(MultiFileColumnMappingMode mapping_mode);

	void ThrowColumnNotFoundError(const string &global_column_name) const;

private:
	ResultColumnMapping CreateColumnMapping(MultiFileColumnMappingMode mapping_mode);
	ResultColumnMapping CreateColumnMappingByMapper(const ColumnMapper &mapper);

	unique_ptr<TableFilterSet> CreateFilters(map<idx_t, reference<TableFilter>> &filters, ResultColumnMapping &mapping);
	ReaderInitializeType EvaluateConstantFilters(ResultColumnMapping &mapping,
	                                             map<idx_t, reference<TableFilter>> &remaining_filters);
	Value GetConstantValue(idx_t global_index);
	bool EvaluateFilterAgainstConstant(TableFilter &filter, const Value &constant);

private:
	ClientContext &context;
	MultiFileReader &multi_file_reader;
	MultiFileList &multi_file_list;
	MultiFileReaderData &reader_data;
	const vector<MultiFileColumnDefinition> &global_columns;
	const vector<ColumnIndex> &global_column_ids;
	optional_ptr<TableFilterSet> global_filters;
	const virtual_column_map_t &virtual_columns;
};

} // namespace goose
