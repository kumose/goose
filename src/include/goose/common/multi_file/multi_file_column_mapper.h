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
