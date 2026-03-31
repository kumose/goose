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

#include <goose/extension/vss/hnsw/hnsw_index_scan.h>

#include <goose/catalog/catalog_entry/goose_table_entry.h>
#include <goose/catalog/dependency_list.h>
#include <goose/main/extension/extension_loader.h>
#include <goose/planner/expression_iterator.h>
#include <goose/planner/operator/logical_get.h>
#include <goose/storage/data_table.h>
#include <goose/storage/index.h>
#include <goose/storage/table/scan_state.h>
#include <goose/storage/statistics/base_statistics.h>
#include <goose/storage/statistics/node_statistics.h>
#include <goose/storage/storage_index.h>
#include <goose/transaction/goose_transaction.h>
#include <goose/transaction/local_storage.h>

#include <goose/extension/vss/hnsw/hnsw.h>
#include <goose/extension/vss/hnsw/hnsw_index.h>

namespace goose {
    BindInfo HNSWIndexScanBindInfo(const optional_ptr<FunctionData> bind_data_p) {
        auto &bind_data = bind_data_p->Cast<HNSWIndexScanBindData>();
        return BindInfo(bind_data.table);
    }

    //-------------------------------------------------------------------------
    // Global State
    //-------------------------------------------------------------------------
    struct HNSWIndexScanGlobalState : public GlobalTableFunctionState {
        ColumnFetchState fetch_state;
        TableScanState local_storage_state;
        vector<StorageIndex> column_ids;

        // Index scan state
        unique_ptr<IndexScanState> index_state;
        Vector row_ids = Vector(LogicalType::ROW_TYPE);

        DataChunk all_columns;
        vector<idx_t> projection_ids;
    };

    static unique_ptr<GlobalTableFunctionState> HNSWIndexScanInitGlobal(ClientContext &context,
                                                                        TableFunctionInitInput &input) {
        auto &bind_data = input.bind_data->Cast<HNSWIndexScanBindData>();

        auto result = make_uniq<HNSWIndexScanGlobalState>();

        // Setup the scan state for the local storage
        auto &local_storage = LocalStorage::Get(context, bind_data.table.catalog);
        result->column_ids.reserve(input.column_ids.size());

        // Figure out the storage column ids
        for (auto &id: input.column_ids) {
            storage_t col_id = id;
            if (id != DConstants::INVALID_INDEX) {
                col_id = bind_data.table.GetColumn(LogicalIndex(id)).StorageOid();
            }
            result->column_ids.emplace_back(col_id);
        }

        // Initialize the storage scan state
        result->local_storage_state.Initialize(result->column_ids, context, input.filters);
        local_storage.InitializeScan(bind_data.table.GetStorage(), result->local_storage_state.local_state,
                                     input.filters);

        // Initialize the scan state for the index
        result->index_state =
                bind_data.index.Cast<HNSWIndex>().InitializeScan(bind_data.query.get(), bind_data.limit, context);

        if (!input.CanRemoveFilterColumns()) {
            return std::move(result);
        }

        // We need this to project out what we scan from the underlying table.
        result->projection_ids = input.projection_ids;

        auto &goose_table = bind_data.table.Cast<GooseTableEntry>();
        const auto &columns = goose_table.GetColumns();
        vector<LogicalType> scanned_types;
        for (const auto &col_idx: input.column_indexes) {
            if (col_idx.IsRowIdColumn()) {
                scanned_types.emplace_back(LogicalType::ROW_TYPE);
            } else {
                scanned_types.push_back(columns.GetColumn(col_idx.ToLogical()).Type());
            }
        }
        result->all_columns.Initialize(context, scanned_types);

        return std::move(result);
    }

    //-------------------------------------------------------------------------
    // Execute
    //-------------------------------------------------------------------------
    static void HNSWIndexScanExecute(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
        auto &bind_data = data_p.bind_data->Cast<HNSWIndexScanBindData>();
        auto &state = data_p.global_state->Cast<HNSWIndexScanGlobalState>();
        auto &transaction = GooseTransaction::Get(context, bind_data.table.catalog);

        // Scan the index for row id's
        auto row_count = bind_data.index.Cast<HNSWIndex>().Scan(*state.index_state, state.row_ids);
        if (row_count == 0) {
            // Short-circuit if the index had no more rows
            output.SetCardinality(0);
            return;
        }

        // Fetch the data from the local storage given the row ids
        if (state.projection_ids.empty()) {
            bind_data.table.GetStorage().Fetch(transaction, output, state.column_ids, state.row_ids, row_count,
                                               state.fetch_state);
            return;
        }

        // Otherwise, we need to first fetch into our scan chunk, and then project out the result
        state.all_columns.Reset();
        bind_data.table.GetStorage().Fetch(transaction, state.all_columns, state.column_ids, state.row_ids, row_count,
                                           state.fetch_state);
        output.ReferenceColumns(state.all_columns, state.projection_ids);
    }

    //-------------------------------------------------------------------------
    // Statistics
    //-------------------------------------------------------------------------
    static unique_ptr<BaseStatistics> HNSWIndexScanStatistics(ClientContext &context, const FunctionData *bind_data_p,
                                                              column_t column_id) {
        auto &bind_data = bind_data_p->Cast<HNSWIndexScanBindData>();
        auto &local_storage = LocalStorage::Get(context, bind_data.table.catalog);
        if (local_storage.Find(bind_data.table.GetStorage())) {
            // we don't emit any statistics for tables that have outstanding transaction-local data
            return nullptr;
        }
        return bind_data.table.GetStatistics(context, column_id);
    }

    //-------------------------------------------------------------------------
    // Dependency
    //-------------------------------------------------------------------------
    void HNSWIndexScanDependency(LogicalDependencyList &entries, const FunctionData *bind_data_p) {
        auto &bind_data = bind_data_p->Cast<HNSWIndexScanBindData>();
        entries.AddDependency(bind_data.table);

        // TODO: Add dependency to index here?
    }

    //-------------------------------------------------------------------------
    // Cardinality
    //-------------------------------------------------------------------------
    unique_ptr<NodeStatistics> HNSWIndexScanCardinality(ClientContext &context, const FunctionData *bind_data_p) {
        auto &bind_data = bind_data_p->Cast<HNSWIndexScanBindData>();
        return make_uniq<NodeStatistics>(bind_data.limit, bind_data.limit);
    }

    //-------------------------------------------------------------------------
    // ToString
    //-------------------------------------------------------------------------
    static InsertionOrderPreservingMap<string> HNSWIndexScanToString(TableFunctionToStringInput &input) {
        D_ASSERT(input.bind_data);
        InsertionOrderPreservingMap<string> result;
        auto &bind_data = input.bind_data->Cast<HNSWIndexScanBindData>();
        result["Table"] = bind_data.table.name;
        result["HNSW Index"] = bind_data.index.GetIndexName();
        return result;
    }

    //-------------------------------------------------------------------------
    // Get Function
    //-------------------------------------------------------------------------
    TableFunction HNSWIndexScanFunction::GetFunction() {
        TableFunction func("hnsw_index_scan", {}, HNSWIndexScanExecute);
        func.init_local = nullptr;
        func.init_global = HNSWIndexScanInitGlobal;
        func.statistics = HNSWIndexScanStatistics;
        func.dependency = HNSWIndexScanDependency;
        func.cardinality = HNSWIndexScanCardinality;
        func.pushdown_complex_filter = nullptr;
        func.to_string = HNSWIndexScanToString;
        func.table_scan_progress = nullptr;
        func.projection_pushdown = true;
        func.filter_pushdown = false;
        func.get_bind_info = HNSWIndexScanBindInfo;

        return func;
    }

    //-------------------------------------------------------------------------
    // Register
    //-------------------------------------------------------------------------
    void HNSWModule::RegisterIndexScan(ExtensionLoader &loader) {
        loader.RegisterFunction(HNSWIndexScanFunction::GetFunction());
    }
} // namespace goose
