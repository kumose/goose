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


#include <goose/planner/operator/logical_vacuum.h>
#include <goose/planner/operator/logical_get.h>
#include <goose/catalog/catalog_entry/table_catalog_entry.h>
#include <goose/common/serializer/serializer.h>
#include <goose/common/serializer/deserializer.h>
#include <goose/parser/parsed_data/vacuum_info.h>

namespace goose {
    LogicalVacuum::LogicalVacuum() : LogicalOperator(LogicalOperatorType::LOGICAL_VACUUM) {
    }

    LogicalVacuum::LogicalVacuum(unique_ptr<VacuumInfo> info)
        : LogicalOperator(LogicalOperatorType::LOGICAL_VACUUM), info(std::move(info)) {
    }

    TableCatalogEntry &LogicalVacuum::GetTable() {
        D_ASSERT(HasTable());
        return *table;
    }

    bool LogicalVacuum::HasTable() const {
        return table != nullptr;
    }

    void LogicalVacuum::SetTable(TableCatalogEntry &table_p) {
        table = &table_p;
    }

    void LogicalVacuum::Serialize(Serializer &serializer) const {
        LogicalOperator::Serialize(serializer);

        serializer.WriteProperty(200, "info", info);
        serializer.WriteProperty(201, "column_id_map", column_id_map);
    }

    unique_ptr<LogicalOperator> LogicalVacuum::Deserialize(Deserializer &deserializer) {
        auto result = unique_ptr<LogicalVacuum>(new LogicalVacuum());

        auto tmp_info = deserializer.ReadPropertyWithDefault<unique_ptr<ParseInfo> >(200, "info");
        deserializer.ReadProperty(201, "column_id_map", result->column_id_map);

        result->info = unique_ptr_cast<ParseInfo, VacuumInfo>(std::move(tmp_info));
        auto &info = *result->info;
        if (info.has_table) {
            // deserialize the 'table'
            auto &context = deserializer.Get<ClientContext &>();
            auto binder = Binder::CreateBinder(context);
            auto bound_table = binder->Bind(*info.ref);
            if (bound_table.plan->type != LogicalOperatorType::LOGICAL_GET) {
                throw InvalidInputException("can only vacuum or analyze base tables");
            }
            auto table_ptr = bound_table.plan->Cast<LogicalGet>().GetTable();
            if (!table_ptr) {
                throw InvalidInputException("can only vacuum or analyze base tables");
            }
            auto &table = *table_ptr;
            result->SetTable(table);
            // FIXME: we should probably verify that the 'column_id_map' and 'columns' are the same on the bound table after
            // deserialization?
        }
        return std::move(result);
    }

    idx_t LogicalVacuum::EstimateCardinality(ClientContext &context) {
        return 1;
    }
} // namespace goose
