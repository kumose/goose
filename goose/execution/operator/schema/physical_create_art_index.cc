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

#include <goose/execution/operator/schema/physical_create_art_index.h>

#include <goose/catalog/catalog_entry/goose_index_entry.h>
#include <goose/catalog/catalog_entry/goose_table_entry.h>
#include <goose/catalog/catalog_entry/table_catalog_entry.h>
#include <goose/execution/index/art/art_key.h>
#include <goose/execution/index/bound_index.h>
#include <goose/main/client_context.h>
#include <goose/main/database_manager.h>
#include <goose/storage/storage_manager.h>
#include <goose/storage/table/append_state.h>
#include <goose/common/exception/transaction_exception.h>
#include <goose/execution/index/art/art_operator.h>

namespace goose {
    PhysicalCreateARTIndex::PhysicalCreateARTIndex(PhysicalPlan &physical_plan, LogicalOperator &op,
                                                   TableCatalogEntry &table_p, const vector<column_t> &column_ids,
                                                   unique_ptr<CreateIndexInfo> info,
                                                   vector<unique_ptr<Expression> > unbound_expressions,
                                                   idx_t estimated_cardinality, const bool sorted,
                                                   unique_ptr<AlterTableInfo> alter_table_info)
        : PhysicalOperator(physical_plan, PhysicalOperatorType::CREATE_INDEX, op.types, estimated_cardinality),
          table(table_p.Cast<GooseTableEntry>()), info(std::move(info)),
          unbound_expressions(std::move(unbound_expressions)),
          sorted(sorted), alter_table_info(std::move(alter_table_info)) {
        // Convert the logical column ids to physical column ids.
        for (auto &column_id: column_ids) {
            storage_ids.push_back(table.GetColumns().LogicalToPhysical(LogicalIndex(column_id)).index);
        }
    }

    //===--------------------------------------------------------------------===//
    // Sink
    //===--------------------------------------------------------------------===//

    class CreateARTIndexGlobalSinkState : public GlobalSinkState {
    public:
        //! We merge the local indexes into one global index.
        unique_ptr<BoundIndex> global_index;
    };

    class CreateARTIndexLocalSinkState : public LocalSinkState {
    public:
        explicit CreateARTIndexLocalSinkState(ClientContext &context) : arena_allocator(Allocator::Get(context)) {
        };

        unique_ptr<BoundIndex> local_index;
        ArenaAllocator arena_allocator;

        DataChunk key_chunk;
        unsafe_vector<ARTKey> keys;
        vector<column_t> key_column_ids;

        DataChunk row_id_chunk;
        unsafe_vector<ARTKey> row_ids;
    };

    unique_ptr<GlobalSinkState> PhysicalCreateARTIndex::GetGlobalSinkState(ClientContext &context) const {
        // Create the global sink state.
        auto state = make_uniq<CreateARTIndexGlobalSinkState>();

        // Create the global index.
        auto &storage = table.GetStorage();
        state->global_index = make_uniq<ART>(info->index_name, info->constraint_type, storage_ids,
                                             TableIOManager::Get(storage), unbound_expressions, storage.db);
        return state;
    }

    unique_ptr<LocalSinkState> PhysicalCreateARTIndex::GetLocalSinkState(ExecutionContext &context) const {
        // Create the local sink state and add the local index.
        auto state = make_uniq<CreateARTIndexLocalSinkState>(context.client);
        auto &storage = table.GetStorage();
        state->local_index = make_uniq<ART>(info->index_name, info->constraint_type, storage_ids,
                                            TableIOManager::Get(storage), unbound_expressions, storage.db);

        // Initialize the local sink state.
        state->keys.resize(STANDARD_VECTOR_SIZE);
        state->row_ids.resize(STANDARD_VECTOR_SIZE);
        state->key_chunk.Initialize(Allocator::Get(context.client), state->local_index->logical_types);
        state->row_id_chunk.Initialize(Allocator::Get(context.client), vector<LogicalType>{LogicalType::ROW_TYPE});
        for (idx_t i = 0; i < state->key_chunk.ColumnCount(); i++) {
            state->key_column_ids.push_back(i);
        }
        return state;
    }

    SinkResultType PhysicalCreateARTIndex::SinkUnsorted(OperatorSinkInput &input) const {
        auto &l_state = input.local_state.Cast<CreateARTIndexLocalSinkState>();
        auto row_count = l_state.key_chunk.size();
        auto &art = l_state.local_index->Cast<ART>();

        // Insert each key and its corresponding row ID.
        for (idx_t i = 0; i < row_count; i++) {
            auto status = art.tree.GetGateStatus();
            auto conflict_type =
                    ARTOperator::Insert(l_state.arena_allocator, art, art.tree, l_state.keys[i], 0, l_state.row_ids[i],
                                        status,
                                        DeleteIndexInfo(), IndexAppendMode::DEFAULT);
            D_ASSERT(conflict_type != ARTConflictType::TRANSACTION);
            if (conflict_type == ARTConflictType::CONSTRAINT) {
                throw ConstraintException("Data contains duplicates on indexed column(s)");
            }
        }

        return SinkResultType::NEED_MORE_INPUT;
    }

    SinkResultType PhysicalCreateARTIndex::SinkSorted(OperatorSinkInput &input) const {
        auto &l_state = input.local_state.Cast<CreateARTIndexLocalSinkState>();
        auto &storage = table.GetStorage();
        auto &l_index = l_state.local_index;

        // Construct an ART for this chunk.
        auto art = make_uniq<ART>(info->index_name, l_index->GetConstraintType(), l_index->GetColumnIds(),
                                  l_index->table_io_manager, l_index->unbound_expressions, storage.db,
                                  l_index->Cast<ART>().allocators);
        if (art->Build(l_state.keys, l_state.row_ids, l_state.key_chunk.size()) != ARTConflictType::NO_CONFLICT) {
            throw ConstraintException("Data contains duplicates on indexed column(s)");
        }

        // Merge the ART into the local ART.
        if (!l_index->MergeIndexes(*art)) {
            throw ConstraintException("Data contains duplicates on indexed column(s)");
        }

        return SinkResultType::NEED_MORE_INPUT;
    }

    SinkResultType PhysicalCreateARTIndex::Sink(ExecutionContext &context, DataChunk &chunk,
                                                OperatorSinkInput &input) const {
        D_ASSERT(chunk.ColumnCount() >= 2);
        auto &l_state = input.local_state.Cast<CreateARTIndexLocalSinkState>();
        l_state.arena_allocator.Reset();
        l_state.key_chunk.ReferenceColumns(chunk, l_state.key_column_ids);

        // Check for NULLs, if we are creating a PRIMARY KEY.
        // FIXME: Later, we want to ensure that we skip the NULL check for any non-PK alter.
        if (alter_table_info) {
            auto row_count = l_state.key_chunk.size();
            for (idx_t i = 0; i < l_state.key_chunk.ColumnCount(); i++) {
                if (VectorOperations::HasNull(l_state.key_chunk.data[i], row_count)) {
                    throw ConstraintException("NOT NULL constraint failed: %s", info->index_name);
                }
            }
        }

        l_state.local_index->Cast<ART>().GenerateKeyVectors(
            l_state.arena_allocator, l_state.key_chunk, chunk.data[chunk.ColumnCount() - 1], l_state.keys,
            l_state.row_ids);

        if (sorted) {
            return SinkSorted(input);
        }
        return SinkUnsorted(input);
    }

    SinkCombineResultType PhysicalCreateARTIndex::Combine(ExecutionContext &context,
                                                          OperatorSinkCombineInput &input) const {
        auto &g_state = input.global_state.Cast<CreateARTIndexGlobalSinkState>();

        // Merge the local index into the global index.
        auto &l_state = input.local_state.Cast<CreateARTIndexLocalSinkState>();
        if (!g_state.global_index->MergeIndexes(*l_state.local_index)) {
            throw ConstraintException("Data contains duplicates on indexed column(s)");
        }

        return SinkCombineResultType::FINISHED;
    }

    SinkFinalizeType PhysicalCreateARTIndex::Finalize(Pipeline &pipeline, Event &event, ClientContext &context,
                                                      OperatorSinkFinalizeInput &input) const {
        auto &state = input.global_state.Cast<CreateARTIndexGlobalSinkState>();

        // Vacuum excess memory and verify.
        state.global_index->Vacuum();
        state.global_index->Verify();
        state.global_index->VerifyAllocations();

        auto &storage = table.GetStorage();
        if (!storage.IsMainTable()) {
            throw TransactionException(
                "Transaction conflict: cannot add an index to a table that has been altered or dropped");
        }

        auto &schema = table.schema;
        info->column_ids = storage_ids;

        if (!alter_table_info) {
            // Ensure that the index does not yet exist in the catalog.
            auto entry = schema.GetEntry(schema.GetCatalogTransaction(context), CatalogType::INDEX_ENTRY,
                                         info->index_name);
            if (entry) {
                if (info->on_conflict != OnCreateConflict::IGNORE_ON_CONFLICT) {
                    throw CatalogException("Index with name \"%s\" already exists!", info->index_name);
                }
                // IF NOT EXISTS on existing index. We are done.
                return SinkFinalizeType::READY;
            }

            auto index_entry = schema.CreateIndex(schema.GetCatalogTransaction(context), *info, table).get();
            D_ASSERT(index_entry);
            auto &index = index_entry->Cast<GooseIndexEntry>();
            index.initial_index_size = state.global_index->GetInMemorySize();
        } else {
            // Ensure that there are no other indexes with that name on this table.
            auto &indexes = storage.GetDataTableInfo()->GetIndexes();
            indexes.Scan([&](Index &index) {
                if (index.GetIndexName() == info->index_name) {
                    throw CatalogException("an index with that name already exists for this table: %s",
                                           info->index_name);
                }
                return false;
            });

            auto &catalog = Catalog::GetCatalog(context, info->catalog);
            catalog.Alter(context, *alter_table_info);
        }

        // Add the index to the storage.
        storage.AddIndex(std::move(state.global_index));
        return SinkFinalizeType::READY;
    }

    //===--------------------------------------------------------------------===//
    // Source
    //===--------------------------------------------------------------------===//

    SourceResultType PhysicalCreateARTIndex::GetDataInternal(ExecutionContext &context, DataChunk &chunk,
                                                             OperatorSourceInput &input) const {
        return SourceResultType::FINISHED;
    }
} // namespace goose
