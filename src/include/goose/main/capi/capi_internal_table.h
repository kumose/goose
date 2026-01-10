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

#include <goose/goose-c.h>
#include <goose/main/capi/capi_internal.h>
#include <goose/function/table_function.h>

namespace goose {

// These need to be shared by both the table function API and the copy function API

struct CTableFunctionInfo : public TableFunctionInfo {
	~CTableFunctionInfo() override {
		if (extra_info && delete_callback) {
			delete_callback(extra_info);
		}
		extra_info = nullptr;
		delete_callback = nullptr;
	}

	goose_table_function_bind_t bind = nullptr;
	goose_table_function_init_t init = nullptr;
	goose_table_function_init_t local_init = nullptr;
	goose_table_function_t function = nullptr;
	void *extra_info = nullptr;
	goose_delete_callback_t delete_callback = nullptr;
};

struct CTableBindData : public TableFunctionData {
	explicit CTableBindData(CTableFunctionInfo &info) : info(info) {
	}
	~CTableBindData() override {
		if (bind_data && delete_callback) {
			delete_callback(bind_data);
		}
		bind_data = nullptr;
		delete_callback = nullptr;
	}

	CTableFunctionInfo &info;
	void *bind_data = nullptr;
	goose_delete_callback_t delete_callback = nullptr;
	unique_ptr<NodeStatistics> stats;
};

struct CTableInternalBindInfo {
	CTableInternalBindInfo(ClientContext &context, const vector<Value> &parameters,
	                       const named_parameter_map_t &named_parameters, vector<LogicalType> &return_types,
	                       vector<string> &names, CTableBindData &bind_data, CTableFunctionInfo &function_info)
	    : context(context), parameters(parameters), named_parameters(named_parameters), return_types(return_types),
	      names(names), bind_data(bind_data), function_info(function_info), success(true) {
	}

	ClientContext &context;

	vector<Value> parameters;
	named_parameter_map_t named_parameters;

	vector<LogicalType> &return_types;
	vector<string> &names;
	CTableBindData &bind_data;
	CTableFunctionInfo &function_info;
	bool success;
	string error;
};

} // namespace goose
