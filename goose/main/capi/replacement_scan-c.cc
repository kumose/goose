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
//

#include <goose/main/capi/capi_internal.h>
#include <goose/main/config.h>
#include <goose/parser/tableref/table_function_ref.h>
#include <goose/parser/expression/constant_expression.h>
#include <goose/parser/expression/function_expression.h>

namespace goose {
    struct CAPIReplacementScanData : public ReplacementScanData {
        ~CAPIReplacementScanData() {
            if (delete_callback) {
                delete_callback(extra_data);
            }
        }

        goose_replacement_callback_t callback;
        void *extra_data;
        goose_delete_callback_t delete_callback;
    };

    struct CAPIReplacementScanInfo {
        CAPIReplacementScanInfo(optional_ptr<CAPIReplacementScanData> data) : data(data) {
        }

        optional_ptr<CAPIReplacementScanData> data;
        string function_name;
        vector<Value> parameters;
        string error;
    };

    unique_ptr<TableRef> goose_capi_replacement_callback(ClientContext &context, ReplacementScanInput &input,
                                                         optional_ptr<ReplacementScanData> data) {
        auto &table_name = input.table_name;
        auto &scan_data = data->Cast<CAPIReplacementScanData>();

        CAPIReplacementScanInfo info(&scan_data);
        scan_data.callback((goose_replacement_scan_info) &info, table_name.c_str(), scan_data.extra_data);
        if (!info.error.empty()) {
            throw BinderException("Error in replacement scan: %s\n", info.error);
        }
        if (info.function_name.empty()) {
            // no function provided: bail-out
            return nullptr;
        }
        auto table_function = make_uniq<TableFunctionRef>();
        vector<unique_ptr<ParsedExpression> > children;
        for (auto &param: info.parameters) {
            children.push_back(make_uniq<ConstantExpression>(std::move(param)));
        }
        table_function->function = make_uniq<FunctionExpression>(info.function_name, std::move(children));
        return table_function;
    }
} // namespace goose

void goose_add_replacement_scan(goose_database db, goose_replacement_callback_t replacement, void *extra_data,
                                goose_delete_callback_t delete_callback) {
    if (!db || !replacement) {
        return;
    }
    auto wrapper = reinterpret_cast<goose::DatabaseWrapper *>(db);
    auto scan_info = goose::make_uniq<goose::CAPIReplacementScanData>();
    scan_info->callback = replacement;
    scan_info->extra_data = extra_data;
    scan_info->delete_callback = delete_callback;

    auto &config = goose::DBConfig::GetConfig(*wrapper->database->instance);
    config.replacement_scans.push_back(
        goose::ReplacementScan(goose::goose_capi_replacement_callback, std::move(scan_info)));
}

void goose_replacement_scan_set_function_name(goose_replacement_scan_info info_p, const char *function_name) {
    if (!info_p || !function_name) {
        return;
    }
    auto info = reinterpret_cast<goose::CAPIReplacementScanInfo *>(info_p);
    info->function_name = function_name;
}

void goose_replacement_scan_add_parameter(goose_replacement_scan_info info_p, goose_value parameter) {
    if (!info_p || !parameter) {
        return;
    }
    auto info = reinterpret_cast<goose::CAPIReplacementScanInfo *>(info_p);
    auto val = reinterpret_cast<goose::Value *>(parameter);
    info->parameters.push_back(*val);
}

void goose_replacement_scan_set_error(goose_replacement_scan_info info_p, const char *error) {
    if (!info_p || !error) {
        return;
    }
    auto info = reinterpret_cast<goose::CAPIReplacementScanInfo *>(info_p);
    info->error = error;
}
