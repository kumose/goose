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

#include <goose/execution/operator/set/physical_cte.h>

#include <goose/common/types/column/column_data_collection.h>
#include <goose/common/vector_operations/vector_operations.h>
#include <goose/execution/aggregate_hashtable.h>
#include <goose/parallel/event.h>
#include <goose/parallel/meta_pipeline.h>
#include <goose/parallel/pipeline.h>

namespace goose {
    PhysicalCTE::PhysicalCTE(PhysicalPlan &physical_plan, string ctename, idx_t table_index, vector<LogicalType> types,
                             PhysicalOperator &top, PhysicalOperator &bottom, idx_t estimated_cardinality)
        : PhysicalOperator(physical_plan, PhysicalOperatorType::CTE, std::move(types), estimated_cardinality),
          table_index(table_index), ctename(std::move(ctename)) {
        children.push_back(top);
        children.push_back(bottom);
    }

    PhysicalCTE::~PhysicalCTE() {
    }

    //===--------------------------------------------------------------------===//
    // Sink
    //===--------------------------------------------------------------------===//
    class CTEGlobalState : public GlobalSinkState {
    public:
        explicit CTEGlobalState(ClientContext &context, const PhysicalCTE &op) : working_table_ref(
            op.working_table.get()) {
        }

        optional_ptr<ColumnDataCollection> working_table_ref;

        mutex lhs_lock;

        void MergeIT(ColumnDataCollection &input) {
            lock_guard<mutex> guard(lhs_lock);
            working_table_ref->Combine(input);
        }
    };

    class CTELocalState : public LocalSinkState {
    public:
        explicit CTELocalState(ClientContext &context, const PhysicalCTE &op)
            : lhs_data(context, op.working_table->Types()) {
            lhs_data.InitializeAppend(append_state);
        }

        unique_ptr<LocalSinkState> distinct_state;
        ColumnDataCollection lhs_data;
        ColumnDataAppendState append_state;

        void Append(DataChunk &input) {
            lhs_data.Append(input);
        }
    };

    unique_ptr<GlobalSinkState> PhysicalCTE::GetGlobalSinkState(ClientContext &context) const {
        working_table->Reset();
        return make_uniq<CTEGlobalState>(context, *this);
    }

    unique_ptr<LocalSinkState> PhysicalCTE::GetLocalSinkState(ExecutionContext &context) const {
        auto state = make_uniq<CTELocalState>(context.client, *this);
        return std::move(state);
    }

    SinkResultType PhysicalCTE::Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const {
        auto &lstate = input.local_state.Cast<CTELocalState>();
        lstate.lhs_data.Append(lstate.append_state, chunk);

        return SinkResultType::NEED_MORE_INPUT;
    }

    SinkCombineResultType PhysicalCTE::Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const {
        auto &lstate = input.local_state.Cast<CTELocalState>();
        auto &gstate = input.global_state.Cast<CTEGlobalState>();
        gstate.MergeIT(lstate.lhs_data);

        return SinkCombineResultType::FINISHED;
    }

    //===--------------------------------------------------------------------===//
    // Pipeline Construction
    //===--------------------------------------------------------------------===//
    void PhysicalCTE::BuildPipelines(Pipeline &current, MetaPipeline &meta_pipeline) {
        D_ASSERT(children.size() == 2);
        op_state.reset();
        sink_state.reset();

        auto &state = meta_pipeline.GetState();

        auto &child_meta_pipeline = meta_pipeline.CreateChildMetaPipeline(current, *this);
        child_meta_pipeline.Build(children[0]);

        for (auto &cte_scan: cte_scans) {
            state.cte_dependencies.insert(
                make_pair(cte_scan, reference<Pipeline>(*child_meta_pipeline.GetBasePipeline())));
        }

        children[1].get().BuildPipelines(current, meta_pipeline);
    }

    vector<const_reference<PhysicalOperator> > PhysicalCTE::GetSources() const {
        return children[1].get().GetSources();
    }

    InsertionOrderPreservingMap<string> PhysicalCTE::ParamsToString() const {
        InsertionOrderPreservingMap<string> result;
        result["CTE Name"] = ctename;
        result["Table Index"] = StringUtil::Format("%llu", table_index);
        SetEstimatedCardinality(result, estimated_cardinality);
        return result;
    }
} // namespace goose
