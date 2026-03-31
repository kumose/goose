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


#include <goose/planner/expression/bound_conjunction_expression.h>
#include <goose/parser/expression/conjunction_expression.h>
#include <goose/parser/expression_util.h>

namespace goose {
    BoundConjunctionExpression::BoundConjunctionExpression(ExpressionType type)
        : Expression(type, ExpressionClass::BOUND_CONJUNCTION, LogicalType::BOOLEAN) {
    }

    BoundConjunctionExpression::BoundConjunctionExpression(ExpressionType type, unique_ptr<Expression> left,
                                                           unique_ptr<Expression> right)
        : BoundConjunctionExpression(type) {
        children.push_back(std::move(left));
        children.push_back(std::move(right));
    }

    string BoundConjunctionExpression::ToString() const {
        return ConjunctionExpression::ToString<BoundConjunctionExpression, Expression>(*this);
    }

    bool BoundConjunctionExpression::Equals(const BaseExpression &other_p) const {
        if (!Expression::Equals(other_p)) {
            return false;
        }
        auto &other = other_p.Cast<BoundConjunctionExpression>();
        return ExpressionUtil::SetEquals(children, other.children);
    }

    bool BoundConjunctionExpression::PropagatesNullValues() const {
        return false;
    }

    unique_ptr<Expression> BoundConjunctionExpression::Copy() const {
        auto copy = make_uniq<BoundConjunctionExpression>(type);
        for (auto &expr: children) {
            copy->children.push_back(expr->Copy());
        }
        copy->CopyProperties(*this);
        return std::move(copy);
    }
} // namespace goose
