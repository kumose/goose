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


#include <goose/catalog/catalog_entry/table_function_catalog_entry.h>
#include <goose/parser/parsed_data/alter_table_function_info.h>
#include <goose/main/attached_database.h>

namespace goose {
    constexpr const char *TableFunctionCatalogEntry::Name;

    TableFunctionCatalogEntry::TableFunctionCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema,
                                                         CreateTableFunctionInfo &info)
        : FunctionEntry(CatalogType::TABLE_FUNCTION_ENTRY, catalog, schema, info),
          functions(std::move(info.functions)) {
        D_ASSERT(this->functions.Size() > 0);
        for (auto &function: functions.functions) {
            function.catalog_name = catalog.GetAttached().GetName();
            function.schema_name = schema.name;
        }
    }

    unique_ptr<CatalogEntry> TableFunctionCatalogEntry::AlterEntry(CatalogTransaction transaction, AlterInfo &info) {
        if (info.type != AlterType::ALTER_TABLE_FUNCTION) {
            throw InternalException("Attempting to alter TableFunctionCatalogEntry with unsupported alter type");
        }
        auto &function_info = info.Cast<AlterTableFunctionInfo>();
        if (function_info.alter_table_function_type != AlterTableFunctionType::ADD_FUNCTION_OVERLOADS) {
            throw InternalException(
                "Attempting to alter TableFunctionCatalogEntry with unsupported alter table function type");
        }
        auto &add_overloads = function_info.Cast<AddTableFunctionOverloadInfo>();

        TableFunctionSet new_set = functions;
        if (!new_set.MergeFunctionSet(add_overloads.new_overloads)) {
            throw BinderException("Failed to add new function overloads to function \"%s\": function already exists",
                                  name);
        }
        CreateTableFunctionInfo new_info(std::move(new_set));
        return make_uniq<TableFunctionCatalogEntry>(catalog, schema, new_info);
    }
} // namespace goose
