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

#pragma once

#include <goose/common/unique_ptr.h>
#include <goose/common/string.h>
#include <goose/common/optional_ptr.h>
#include <goose/main/query_profiler.h>
#include <goose/main/profiling_node.h>

namespace goose {
    class LogicalOperator;
    class PhysicalOperator;
    class Pipeline;
    struct PipelineRenderNode;

    struct RenderTreeNode {
    public:
        static constexpr const char *CARDINALITY = "__cardinality__";
        static constexpr const char *ESTIMATED_CARDINALITY = "__estimated_cardinality__";
        static constexpr const char *TIMING = "__timing__";

    public:
        struct Coordinate {
        public:
            Coordinate(idx_t x, idx_t y) : x(x), y(y) {
            }

        public:
            idx_t x;
            idx_t y;
        };

        RenderTreeNode(const string &name, InsertionOrderPreservingMap<string> extra_text)
            : name(name), extra_text(std::move(extra_text)) {
        }

    public:
        void AddChildPosition(idx_t x, idx_t y) {
            child_positions.emplace_back(x, y);
        }

    public:
        string name;
        InsertionOrderPreservingMap<string> extra_text;
        vector<Coordinate> child_positions;
    };

    struct RenderTree {
        RenderTree(idx_t width, idx_t height);

        unique_array<unique_ptr<RenderTreeNode> > nodes;
        idx_t width;
        idx_t height;

    public:
        static unique_ptr<RenderTree> CreateRenderTree(const LogicalOperator &op);

        static unique_ptr<RenderTree> CreateRenderTree(const PhysicalOperator &op);

        static unique_ptr<RenderTree> CreateRenderTree(const ProfilingNode &op);

        static unique_ptr<RenderTree> CreateRenderTree(const Pipeline &op);

    public:
        optional_ptr<RenderTreeNode> GetNode(idx_t x, idx_t y);

        void SetNode(idx_t x, idx_t y, unique_ptr<RenderTreeNode> node);

        bool HasNode(idx_t x, idx_t y);

        void SanitizeKeyNames();

    private:
        idx_t GetPosition(idx_t x, idx_t y);
    };
} // namespace goose
