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


#include <goose/catalog/catalog.h>
#include <goose/catalog/catalog_entry/type_catalog_entry.h>
#include <goose/catalog/catalog_entry/schema_catalog_entry.h>
#include <goose/common/exception.h>
#include <goose/common/limits.h>
#include <goose/parser/keyword_helper.h>
#include <algorithm>
#include <sstream>

namespace goose {
    constexpr const char *TypeCatalogEntry::Name;

    TypeCatalogEntry::TypeCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateTypeInfo &info)
        : StandardEntry(CatalogType::TYPE_ENTRY, schema, catalog, info.name), user_type(info.type),
          bind_function(info.bind_function) {
        this->temporary = info.temporary;
        this->internal = info.internal;
        this->dependencies = info.dependencies;
        this->comment = info.comment;
        this->tags = info.tags;
    }

    unique_ptr<CatalogEntry> TypeCatalogEntry::Copy(ClientContext &context) const {
        auto info_copy = GetInfo();
        auto &cast_info = info_copy->Cast<CreateTypeInfo>();
        auto result = make_uniq<TypeCatalogEntry>(catalog, schema, cast_info);
        return result;
    }

    unique_ptr<CreateInfo> TypeCatalogEntry::GetInfo() const {
        auto result = make_uniq<CreateTypeInfo>();
        result->catalog = catalog.GetName();
        result->schema = schema.name;
        result->name = name;
        result->type = user_type;
        result->dependencies = dependencies;
        result->comment = comment;
        result->tags = tags;
        result->bind_function = bind_function;
        return result;
    }

    string TypeCatalogEntry::ToSQL() const {
        goose::stringstream ss;
        ss << "CREATE TYPE ";
        ss << KeywordHelper::WriteOptionallyQuoted(name);
        ss << " AS ";

        auto user_type_copy = user_type;

        // Strip off the potential alias so ToString doesn't just output the alias
        user_type_copy.SetAlias("");
        D_ASSERT(user_type_copy.GetAlias().empty());

        ss << user_type_copy.ToString();
        ss << ";";
        return ss.str();
    }
} // namespace goose
