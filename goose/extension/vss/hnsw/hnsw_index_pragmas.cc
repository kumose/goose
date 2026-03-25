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
#include <goose/catalog/catalog_entry.h>
#include <goose/catalog/catalog_entry/index_catalog_entry.h>
#include <goose/catalog/catalog_entry/schema_catalog_entry.h>
#include <goose/catalog/catalog_entry/table_catalog_entry.h>
#include <goose/common/enums/catalog_type.h>
#include <goose/common/exception/binder_exception.h>
#include <goose/common/helper.h>
#include <goose/common/optional_ptr.h>
#include <goose/common/string.h>
#include <goose/common/typedefs.h>
#include <goose/common/types.h>
#include <goose/common/types/data_chunk.h>
#include <goose/common/types/value.h>
#include <goose/common/types/vector.h>
#include <goose/common/unique_ptr.h>
#include <goose/common/vector.h>
#include <goose/common/vector_size.h>
#include <goose/function/function.h>
#include <goose/function/pragma_function.h>
#include <goose/function/table_function.h>
#include <goose/main/extension/extension_loader.h>
#include <goose/parser/qualified_name.h>
#include <goose/planner/binder.h>
#include <goose/planner/expression_iterator.h>
#include <goose/planner/operator/logical_get.h>
#include <goose/storage/data_table.h>
#include <goose/storage/index.h>
#include <goose/storage/table/data_table_info.h>
#include <goose/storage/table/table_index_list.h>
#include <goose/transaction/goose_transaction.h>
#include <goose/transaction/local_storage.h>

#include <goose/extension/vss/hnsw/hnsw.h>
#include <goose/extension/vss/hnsw/hnsw_index.h>
#include <goose/extension/vss/hnsw/hnsw_index_scan.h>

namespace goose {
    // BIND
    static unique_ptr<FunctionData> HNSWindexInfoBind(ClientContext &context, TableFunctionBindInput &input,
                                                      vector<LogicalType> &return_types, vector<string> &names) {
        names.emplace_back("catalog_name");
        return_types.emplace_back(LogicalType::VARCHAR);

        names.emplace_back("schema_name");
        return_types.emplace_back(LogicalType::VARCHAR);

        names.emplace_back("index_name");
        return_types.emplace_back(LogicalType::VARCHAR);

        names.emplace_back("table_name");
        return_types.emplace_back(LogicalType::VARCHAR);

        names.emplace_back("metric");
        return_types.emplace_back(LogicalType::VARCHAR);

        names.emplace_back("dimensions");
        return_types.emplace_back(LogicalType::BIGINT);

        names.emplace_back("count");
        return_types.emplace_back(LogicalType::BIGINT);

        names.emplace_back("capacity");
        return_types.emplace_back(LogicalType::BIGINT);

        names.emplace_back("approx_memory_usage");
        return_types.emplace_back(LogicalType::BIGINT);

        names.emplace_back("levels");
        return_types.emplace_back(LogicalType::BIGINT);

        names.emplace_back("levels_stats");
        return_types.emplace_back(LogicalType::LIST(LogicalType::STRUCT({
            {"nodes", LogicalType::BIGINT},
            {"edges", LogicalType::BIGINT},
            {"max_edges", LogicalType::BIGINT},
            {"allocated_bytes", LogicalType::BIGINT}
        })));

        return nullptr;
    }

    // INIT GLOBAL
    struct HNSWIndexInfoGlobalState : public GlobalTableFunctionState {
        idx_t offset = 0;
        vector<reference<IndexCatalogEntry> > entries;
    };

    static unique_ptr<GlobalTableFunctionState> HNSWIndexInfoInitGlobal(ClientContext &context,
                                                                        TableFunctionInitInput &input) {
        auto result = make_uniq<HNSWIndexInfoGlobalState>();

        // scan all the schemas for indexes and collect them
        auto schemas = Catalog::GetAllSchemas(context);
        for (auto &schema: schemas) {
            schema.get().Scan(context, CatalogType::INDEX_ENTRY, [&](CatalogEntry &entry) {
                auto &index_entry = entry.Cast<IndexCatalogEntry>();
                if (index_entry.index_type == HNSWIndex::TYPE_NAME) {
                    result->entries.push_back(index_entry);
                }
            });
        };
        return result;
    }

    // EXECUTE
    static void HNSWIndexInfoExecute(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
        auto &data = data_p.global_state->Cast<HNSWIndexInfoGlobalState>();
        if (data.offset >= data.entries.size()) {
            return;
        }

        idx_t row = 0;
        while (data.offset < data.entries.size() && row < STANDARD_VECTOR_SIZE) {
            auto &index_entry = data.entries[data.offset++].get();
            auto &table_entry = index_entry.schema.catalog.GetEntry<TableCatalogEntry>(
                context, index_entry.GetSchemaName(),
                index_entry.GetTableName());
            auto &storage = table_entry.GetStorage();
            HNSWIndex *hnsw_index = nullptr;

            auto &table_info = *storage.GetDataTableInfo();

            table_info.BindIndexes(context, HNSWIndex::TYPE_NAME);
            table_info.GetIndexes().Scan([&](Index &index) {
                if (!index.IsBound() || HNSWIndex::TYPE_NAME != index.GetIndexType()) {
                    return false;
                }
                auto &cast_index = index.Cast<HNSWIndex>();
                if (cast_index.name == index_entry.name) {
                    hnsw_index = &cast_index;
                    return true;
                }
                return false;
            });

            if (!hnsw_index) {
                throw BinderException("Index %s not found", index_entry.name);
            }

            idx_t col = 0;

            output.data[col++].SetValue(row, Value(index_entry.catalog.GetName()));
            output.data[col++].SetValue(row, Value(index_entry.schema.name));
            output.data[col++].SetValue(row, Value(index_entry.name));
            output.data[col++].SetValue(row, Value(table_entry.name));

            auto stats = hnsw_index->GetStats();

            output.data[col++].SetValue(row, Value(hnsw_index->GetMetric()));
            output.data[col++].SetValue(row, Value::BIGINT(hnsw_index->GetVectorSize()));
            output.data[col++].SetValue(row, Value::BIGINT(stats->count));
            output.data[col++].SetValue(row, Value::BIGINT(stats->capacity));
            output.data[col++].SetValue(row, Value::BIGINT(stats->approx_size));
            output.data[col++].SetValue(row, Value::BIGINT(stats->max_level));

            vector<Value> level_stats;
            for (auto &stat: stats->level_stats) {
                level_stats.push_back(Value::STRUCT({
                    {"nodes", Value::BIGINT(stat.nodes)},
                    {"edges", Value::BIGINT(stat.edges)},
                    {"max_edges", Value::BIGINT(stat.max_edges)},
                    {"allocated_bytes", Value::BIGINT(stat.allocated_bytes)}
                }));
            }
            auto level_stat_value = Value::LIST(LogicalType::STRUCT({
                                                    {
                                                        {"nodes", LogicalType::BIGINT},
                                                        {"edges", LogicalType::BIGINT},
                                                        {"max_edges", LogicalType::BIGINT},
                                                        {"allocated_bytes", LogicalType::BIGINT}
                                                    }
                                                }),
                                                level_stats);

            output.data[col++].SetValue(row, level_stat_value);

            row++;
        }
        output.SetCardinality(row);
    }

    //-------------------------------------------------------------------------
    // Compact PRAGMA
    //-------------------------------------------------------------------------

    static void CompactIndexPragma(ClientContext &context, const FunctionParameters &parameters) {
        if (parameters.values.size() != 1) {
            throw BinderException("Expected one argument for hnsw_compact_index");
        }
        auto &param = parameters.values[0];
        if (param.type() != LogicalType::VARCHAR) {
            throw BinderException("Expected a string argument for hnsw_compact_index");
        }
        auto index_name = param.GetValue<string>();

        auto qname = QualifiedName::Parse(index_name);

        // look up the index name in the catalog
        Binder::BindSchemaOrCatalog(context, qname.catalog, qname.schema);
        auto &index_entry = Catalog::GetEntry(context, CatalogType::INDEX_ENTRY, qname.catalog, qname.schema,
                                              qname.name)
                .Cast<IndexCatalogEntry>();
        auto &table_entry = Catalog::GetEntry(context, CatalogType::TABLE_ENTRY, qname.catalog,
                                              index_entry.GetSchemaName(),
                                              index_entry.GetTableName())
                .Cast<TableCatalogEntry>();

        auto &storage = table_entry.GetStorage();
        bool found_index = false;

        auto &table_info = *storage.GetDataTableInfo();
        table_info.BindIndexes(context, HNSWIndex::TYPE_NAME);
        table_info.GetIndexes().Scan([&](Index &index) {
            if (!index.IsBound() || HNSWIndex::TYPE_NAME != index.GetIndexType()) {
                return false;
            }
            auto &cast_index = index.Cast<HNSWIndex>();
            if (cast_index.name == index_entry.name) {
                cast_index.Compact();
                found_index = true;
                return true;
            }
            return false;
        });

        if (!found_index) {
            throw BinderException("Index %s not found", index_name);
        }
    }

    //-------------------------------------------------------------------------
    // Register
    //-------------------------------------------------------------------------
    void HNSWModule::RegisterIndexPragmas(ExtensionLoader &loader) {
        loader.RegisterFunction(
            PragmaFunction::PragmaCall("hnsw_compact_index", CompactIndexPragma, {LogicalType::VARCHAR}));

        // TODO: This is kind of ugly and maybe should just take a parameter instead...
        TableFunction info_function("pragma_hnsw_index_info", {}, HNSWIndexInfoExecute, HNSWindexInfoBind,
                                    HNSWIndexInfoInitGlobal);
        loader.RegisterFunction(info_function);
    }
} // namespace goose
