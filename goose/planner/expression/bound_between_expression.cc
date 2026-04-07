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


#include <goose/planner/expression/bound_between_expression.h>
#include <goose/parser/expression/between_expression.h>

namespace goose {
    BoundBetweenExpression::BoundBetweenExpression()
        : Expression(ExpressionType::COMPARE_BETWEEN, ExpressionClass::BOUND_BETWEEN, LogicalType::BOOLEAN) {
    }

    BoundBetweenExpression::BoundBetweenExpression(unique_ptr<Expression> input, unique_ptr<Expression> lower,
                                                   unique_ptr<Expression> upper, bool lower_inclusive,
                                                   bool upper_inclusive)
        : Expression(ExpressionType::COMPARE_BETWEEN, ExpressionClass::BOUND_BETWEEN, LogicalType::BOOLEAN),
          input(std::move(input)), lower(std::move(lower)), upper(std::move(upper)), lower_inclusive(lower_inclusive),
          upper_inclusive(upper_inclusive) {
    }

    string BoundBetweenExpression::ToString() const {
        return BetweenExpression::ToString<BoundBetweenExpression, Expression>(*this);
    }

    bool BoundBetweenExpression::Equals(const BaseExpression &other_p) const {
        if (!Expression::Equals(other_p)) {
            return false;
        }
        auto &other = other_p.Cast<BoundBetweenExpression>();
        if (!Expression::Equals(*input, *other.input)) {
            return false;
        }
        if (!Expression::Equals(*lower, *other.lower)) {
            return false;
        }
        if (!Expression::Equals(*upper, *other.upper)) {
            return false;
        }
        return lower_inclusive == other.lower_inclusive && upper_inclusive == other.upper_inclusive;
    }

    unique_ptr<Expression> BoundBetweenExpression::Copy() const {
        auto copy = make_uniq<BoundBetweenExpression>(input->Copy(), lower->Copy(), upper->Copy(), lower_inclusive,
                                                      upper_inclusive);
        copy->CopyProperties(*this);
        return std::move(copy);
    }
} // namespace goose
