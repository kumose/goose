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


#include <goose/extension/json/json_common.h>
#include <goose/extension/json/json_functions.h>
#include <goose/extension/json/json_scan.h>
#include <goose/common/helper.h>
#include <goose/extension/json/json_multi_file_info.h>

namespace goose {
    TableFunction GetReadJSONObjectsTableFunction(string name, shared_ptr<JSONScanInfo> function_info) {
        MultiFileFunction<JSONMultiFileInfo> table_function(std::move(name));
        JSONScan::TableFunctionDefaults(table_function);
        table_function.function_info = std::move(function_info);
        return static_cast<TableFunction>(table_function);
    }

    TableFunctionSet JSONFunctions::GetReadJSONObjectsFunction() {
        auto function_info = make_shared_ptr<JSONScanInfo>(JSONScanType::READ_JSON_OBJECTS, JSONFormat::AUTO_DETECT,
                                                           JSONRecordType::RECORDS);
        auto table_function = GetReadJSONObjectsTableFunction("read_json_objects", std::move(function_info));
        return MultiFileReader::CreateFunctionSet(std::move(table_function));
    }

    TableFunctionSet JSONFunctions::GetReadNDJSONObjectsFunction() {
        auto function_info = make_shared_ptr<JSONScanInfo>(JSONScanType::READ_JSON_OBJECTS,
                                                           JSONFormat::NEWLINE_DELIMITED,
                                                           JSONRecordType::RECORDS);
        auto table_function = GetReadJSONObjectsTableFunction("read_ndjson_objects", std::move(function_info));
        return MultiFileReader::CreateFunctionSet(std::move(table_function));
    }

    TableFunctionSet JSONFunctions::GetReadJSONObjectsAutoFunction() {
        auto function_info = make_shared_ptr<JSONScanInfo>(JSONScanType::READ_JSON_OBJECTS, JSONFormat::AUTO_DETECT,
                                                           JSONRecordType::RECORDS);
        auto table_function = GetReadJSONObjectsTableFunction("read_json_objects_auto", std::move(function_info));
        return MultiFileReader::CreateFunctionSet(std::move(table_function));
    }
} // namespace goose
