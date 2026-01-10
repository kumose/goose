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


#include "json_common.h"
#include "json_functions.h"
#include "json_scan.h"
#include <goose/common/helper.h>
#include "json_multi_file_info.h"

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
