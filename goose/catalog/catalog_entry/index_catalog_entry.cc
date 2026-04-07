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

#include <goose/catalog/catalog_entry/index_catalog_entry.h>

namespace goose {
    IndexCatalogEntry::IndexCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateIndexInfo &info)
        : StandardEntry(CatalogType::INDEX_ENTRY, schema, catalog, info.index_name), sql(info.sql),
          options(info.options),
          index_type(info.index_type), index_constraint_type(info.constraint_type), column_ids(info.column_ids) {
        this->temporary = info.temporary;
        this->dependencies = info.dependencies;
        this->comment = info.comment;
        for (auto &expr: expressions) {
            D_ASSERT(expr);
            expressions.push_back(expr->Copy());
        }
        for (auto &parsed_expr: info.parsed_expressions) {
            D_ASSERT(parsed_expr);
            parsed_expressions.push_back(parsed_expr->Copy());
        }
    }

    unique_ptr<CreateInfo> IndexCatalogEntry::GetInfo() const {
        auto result = make_uniq<CreateIndexInfo>();
        result->schema = GetSchemaName();
        result->table = GetTableName();

        result->temporary = temporary;
        result->sql = sql;
        result->index_name = name;
        result->index_type = index_type;
        result->constraint_type = index_constraint_type;
        result->column_ids = column_ids;
        result->dependencies = dependencies;

        for (auto &expr: expressions) {
            result->expressions.push_back(expr->Copy());
        }
        for (auto &expr: parsed_expressions) {
            result->parsed_expressions.push_back(expr->Copy());
        }

        result->comment = comment;
        result->tags = tags;

        return std::move(result);
    }

    string IndexCatalogEntry::ToSQL() const {
        auto info = GetInfo();
        return info->ToString();
    }

    bool IndexCatalogEntry::IsUnique() const {
        return (index_constraint_type == IndexConstraintType::UNIQUE ||
                index_constraint_type == IndexConstraintType::PRIMARY);
    }

    bool IndexCatalogEntry::IsPrimary() const {
        return (index_constraint_type == IndexConstraintType::PRIMARY);
    }
} // namespace goose
