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

#pragma once

#include <goose/common/enums/operator_result_type.h>
#include <goose/common/enums/physical_operator_type.h>
#include <goose/common/typedefs.h>
#include <goose/common/types.h>
#include <goose/common/unique_ptr.h>
#include <goose/common/vector.h>
#include <goose/execution/physical_operator.h>
#include <goose/parser/parsed_data/create_index_info.h>
#include <goose/planner/expression.h>
#include <goose/storage/buffer_manager.h>
#include <goose/storage/index.h>
#include <goose/storage/index_storage_info.h>

namespace goose {
    class GooseTableEntry;

    class PhysicalCreateHNSWIndex : public PhysicalOperator {
    public:
        static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::EXTENSION;

    public:
        PhysicalCreateHNSWIndex(PhysicalPlan &physical_plan, const vector<LogicalType> &types_p,
                                TableCatalogEntry &table,
                                const vector<column_t> &column_ids, unique_ptr<CreateIndexInfo> info,
                                vector<unique_ptr<Expression> > unbound_expressions, idx_t estimated_cardinality);

        //! The table to create the index for
        GooseTableEntry &table;
        //! The list of column IDs required for the index
        vector<column_t> storage_ids;
        //! Info for index creation
        unique_ptr<CreateIndexInfo> info;
        //! Unbound expressions to be used in the optimizer
        vector<unique_ptr<Expression> > unbound_expressions;
        //! Whether the pipeline sorts the data prior to index creation
        const bool sorted;

    public:
        //! Source interface, NOOP for this operator
        SourceResultType
        GetData(ExecutionContext &context, DataChunk &chunk, OperatorSourceInput &input) const override {
            return SourceResultType::FINISHED;
        }

        bool IsSource() const override {
            return true;
        }

    public:
        //! Sink interface, thread-local sink states
        unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;

        //! Sink interface, global sink state
        unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;

        SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;

        SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;

        SinkFinalizeType Finalize(Pipeline &pipeline, Event &event, ClientContext &context,
                                  OperatorSinkFinalizeInput &input) const override;

        bool IsSink() const override {
            return true;
        }

        bool ParallelSink() const override {
            return true;
        }

        ProgressData GetSinkProgress(ClientContext &context, GlobalSinkState &gstate,
                                     ProgressData source_progress) const override;
    };
} // namespace goose
