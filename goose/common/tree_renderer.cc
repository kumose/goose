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


#include <goose/common/tree_renderer.h>
#include <goose/common/tree_renderer/text_tree_renderer.h>
#include <goose/common/tree_renderer/json_tree_renderer.h>
#include <goose/common/tree_renderer/html_tree_renderer.h>
#include <goose/common/tree_renderer/graphviz_tree_renderer.h>
#include <goose/common/tree_renderer/yaml_tree_renderer.h>
#include <goose/common/tree_renderer/mermaid_tree_renderer.h>

#include <sstream>

namespace goose {
    unique_ptr<TreeRenderer> TreeRenderer::CreateRenderer(ExplainFormat format) {
        switch (format) {
            case ExplainFormat::DEFAULT:
            case ExplainFormat::TEXT:
                return make_uniq<TextTreeRenderer>();
            case ExplainFormat::JSON:
                return make_uniq<JSONTreeRenderer>();
            case ExplainFormat::HTML:
                return make_uniq<HTMLTreeRenderer>();
            case ExplainFormat::GRAPHVIZ:
                return make_uniq<GRAPHVIZTreeRenderer>();
            case ExplainFormat::YAML:
                return make_uniq<YAMLTreeRenderer>();
            case ExplainFormat::MERMAID:
                return make_uniq<MermaidTreeRenderer>();
            default:
                throw NotImplementedException("ExplainFormat %s not implemented", EnumUtil::ToString(format));
        }
    }
} // namespace goose
