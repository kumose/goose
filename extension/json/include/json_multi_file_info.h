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

#include <goose/common/multi_file/multi_file_function.h>
#include "json_reader_options.h"

namespace goose {

class JSONFileReaderOptions : public BaseFileReaderOptions {
public:
	JSONReaderOptions options;
};

struct JSONMultiFileInfo : MultiFileReaderInterface {
	static unique_ptr<MultiFileReaderInterface> CreateInterface(ClientContext &context);

	unique_ptr<BaseFileReaderOptions> InitializeOptions(ClientContext &context,
	                                                    optional_ptr<TableFunctionInfo> info) override;
	bool ParseCopyOption(ClientContext &context, const string &key, const vector<Value> &values,
	                     BaseFileReaderOptions &options, vector<string> &expected_names,
	                     vector<LogicalType> &expected_types) override;
	bool ParseOption(ClientContext &context, const string &key, const Value &val, MultiFileOptions &file_options,
	                 BaseFileReaderOptions &options) override;
	void FinalizeCopyBind(ClientContext &context, BaseFileReaderOptions &options, const vector<string> &expected_names,
	                      const vector<LogicalType> &expected_types) override;
	unique_ptr<TableFunctionData> InitializeBindData(MultiFileBindData &multi_file_data,
	                                                 unique_ptr<BaseFileReaderOptions> options) override;
	void BindReader(ClientContext &context, vector<LogicalType> &return_types, vector<string> &names,
	                MultiFileBindData &bind_data) override;
	optional_idx MaxThreads(const MultiFileBindData &bind_data, const MultiFileGlobalState &global_state,
	                        FileExpandResult expand_result) override;
	unique_ptr<GlobalTableFunctionState> InitializeGlobalState(ClientContext &context, MultiFileBindData &bind_data,
	                                                           MultiFileGlobalState &global_state) override;
	unique_ptr<LocalTableFunctionState> InitializeLocalState(ExecutionContext &context,
	                                                         GlobalTableFunctionState &global_state) override;
	shared_ptr<BaseFileReader> CreateReader(ClientContext &context, GlobalTableFunctionState &gstate,
	                                        BaseUnionData &union_data, const MultiFileBindData &bind_data_p) override;
	shared_ptr<BaseFileReader> CreateReader(ClientContext &context, GlobalTableFunctionState &gstate,
	                                        const OpenFileInfo &file, idx_t file_idx,
	                                        const MultiFileBindData &bind_data) override;
	void FinishReading(ClientContext &context, GlobalTableFunctionState &global_state,
	                   LocalTableFunctionState &local_state) override;
	unique_ptr<NodeStatistics> GetCardinality(const MultiFileBindData &bind_data, idx_t file_count) override;
	FileGlobInput GetGlobInput() override;
};

} // namespace goose
