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
