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


#include <goose/common/exception.h>
#include <goose/parser/transformer.h>
#include <goose/parser/expression/positional_reference_expression.h>

namespace goose {
    unique_ptr<ParsedExpression> Transformer::TransformPositionalReference(cantor::PGPositionalReference &node) {
        if (node.position <= 0) {
            throw ParserException("Positional reference node needs to be >= 1");
        }
        auto result = make_uniq<PositionalReferenceExpression>(NumericCast<idx_t>(node.position));
        SetQueryLocation(*result, node.location);
        return result;
    }
} // namespace goose
