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

#include <goose/catalog/catalog_entry/scalar_macro_catalog_entry.h>
#include <goose/catalog/catalog_entry/table_macro_catalog_entry.h>
#include <goose/function/scalar_macro_function.h>

namespace goose {
    MacroCatalogEntry::MacroCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateMacroInfo &info)
        : FunctionEntry(
              (info.macros[0]->type == MacroType::SCALAR_MACRO
                   ? CatalogType::MACRO_ENTRY
                   : CatalogType::TABLE_MACRO_ENTRY),
              catalog, schema, info),
          macros(std::move(info.macros)) {
        this->temporary = info.temporary;
        this->internal = info.internal;
        this->dependencies = info.dependencies;
        this->comment = info.comment;
        this->tags = info.tags;
    }

    ScalarMacroCatalogEntry::ScalarMacroCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema,
                                                     CreateMacroInfo &info)
        : MacroCatalogEntry(catalog, schema, info) {
    }

    unique_ptr<CatalogEntry> ScalarMacroCatalogEntry::Copy(ClientContext &context) const {
        auto info_copy = GetInfo();
        auto &cast_info = info_copy->Cast<CreateMacroInfo>();
        auto result = make_uniq<ScalarMacroCatalogEntry>(catalog, schema, cast_info);
        return result;
    }

    TableMacroCatalogEntry::TableMacroCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateMacroInfo &info)
        : MacroCatalogEntry(catalog, schema, info) {
    }

    unique_ptr<CatalogEntry> TableMacroCatalogEntry::Copy(ClientContext &context) const {
        auto info_copy = GetInfo();
        auto &cast_info = info_copy->Cast<CreateMacroInfo>();
        auto result = make_uniq<TableMacroCatalogEntry>(catalog, schema, cast_info);
        return result;
    }

    unique_ptr<CreateInfo> MacroCatalogEntry::GetInfo() const {
        auto info = make_uniq<CreateMacroInfo>(type);
        info->catalog = catalog.GetName();
        info->schema = schema.name;
        info->name = name;
        for (auto &function: macros) {
            info->macros.push_back(function->Copy());
        }
        info->dependencies = dependencies;
        info->comment = comment;
        info->tags = tags;
        return info;
    }

    string MacroCatalogEntry::ToSQL() const {
        auto create_info = GetInfo();
        return create_info->ToString();
    }
} // namespace goose
