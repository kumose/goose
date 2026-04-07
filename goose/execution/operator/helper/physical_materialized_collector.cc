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

#include <goose/execution/operator/helper/physical_materialized_collector.h>

#include <goose/main/materialized_query_result.h>
#include <goose/main/client_context.h>
#include <goose/main/result_set_manager.h>

namespace goose {
    PhysicalMaterializedCollector::PhysicalMaterializedCollector(PhysicalPlan &physical_plan,
                                                                 PreparedStatementData &data,
                                                                 bool parallel)
        : PhysicalResultCollector(physical_plan, data), parallel(parallel) {
    }

    class MaterializedCollectorGlobalState : public GlobalSinkState {
    public:
        mutex glock;
        unique_ptr<ColumnDataCollection> collection;
        shared_ptr<ClientContext> context;
    };

    class MaterializedCollectorLocalState : public LocalSinkState {
    public:
        unique_ptr<ColumnDataCollection> collection;
        ColumnDataAppendState append_state;
    };

    SinkResultType PhysicalMaterializedCollector::Sink(ExecutionContext &context, DataChunk &chunk,
                                                       OperatorSinkInput &input) const {
        auto &lstate = input.local_state.Cast<MaterializedCollectorLocalState>();
        lstate.collection->Append(lstate.append_state, chunk);
        return SinkResultType::NEED_MORE_INPUT;
    }

    SinkCombineResultType PhysicalMaterializedCollector::Combine(ExecutionContext &context,
                                                                 OperatorSinkCombineInput &input) const {
        auto &gstate = input.global_state.Cast<MaterializedCollectorGlobalState>();
        auto &lstate = input.local_state.Cast<MaterializedCollectorLocalState>();
        if (lstate.collection->Count() == 0) {
            return SinkCombineResultType::FINISHED;
        }

        lock_guard<mutex> l(gstate.glock);
        if (!gstate.collection) {
            gstate.collection = std::move(lstate.collection);
        } else {
            gstate.collection->Combine(*lstate.collection);
        }

        return SinkCombineResultType::FINISHED;
    }

    unique_ptr<GlobalSinkState> PhysicalMaterializedCollector::GetGlobalSinkState(ClientContext &context) const {
        auto state = make_uniq<MaterializedCollectorGlobalState>();
        state->context = context.shared_from_this();
        return std::move(state);
    }

    unique_ptr<LocalSinkState> PhysicalMaterializedCollector::GetLocalSinkState(ExecutionContext &context) const {
        auto state = make_uniq<MaterializedCollectorLocalState>();
        state->collection = CreateCollection(context.client);
        state->collection->InitializeAppend(state->append_state);
        return std::move(state);
    }

    unique_ptr<QueryResult> PhysicalMaterializedCollector::GetResult(GlobalSinkState &state) const {
        auto &gstate = state.Cast<MaterializedCollectorGlobalState>();
        if (!gstate.collection) {
            gstate.collection = CreateCollection(*gstate.context);
        }
        auto result = make_uniq<MaterializedQueryResult>(statement_type, properties, names,
                                                         std::move(gstate.collection),
                                                         gstate.context->GetClientProperties());
        return std::move(result);
    }

    bool PhysicalMaterializedCollector::ParallelSink() const {
        return parallel;
    }

    bool PhysicalMaterializedCollector::SinkOrderDependent() const {
        return true;
    }
} // namespace goose
