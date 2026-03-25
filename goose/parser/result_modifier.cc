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


#include <goose/parser/result_modifier.h>
#include <goose/parser/expression_util.h>

namespace goose {
    bool ResultModifier::Equals(const ResultModifier &other) const {
        return type == other.type;
    }

    bool LimitModifier::Equals(const ResultModifier &other_p) const {
        if (!ResultModifier::Equals(other_p)) {
            return false;
        }
        auto &other = other_p.Cast<LimitModifier>();
        if (!ParsedExpression::Equals(limit, other.limit)) {
            return false;
        }
        if (!ParsedExpression::Equals(offset, other.offset)) {
            return false;
        }
        return true;
    }

    unique_ptr<ResultModifier> LimitModifier::Copy() const {
        auto copy = make_uniq<LimitModifier>();
        if (limit) {
            copy->limit = limit->Copy();
        }
        if (offset) {
            copy->offset = offset->Copy();
        }
        return copy;
    }

    bool DistinctModifier::Equals(const ResultModifier &other_p) const {
        if (!ResultModifier::Equals(other_p)) {
            return false;
        }
        auto &other = other_p.Cast<DistinctModifier>();
        if (!ExpressionUtil::ListEquals(distinct_on_targets, other.distinct_on_targets)) {
            return false;
        }
        return true;
    }

    unique_ptr<ResultModifier> DistinctModifier::Copy() const {
        auto copy = make_uniq<DistinctModifier>();
        for (auto &expr: distinct_on_targets) {
            copy->distinct_on_targets.push_back(expr->Copy());
        }
        return copy;
    }

    bool OrderModifier::Equals(const ResultModifier &other_p) const {
        if (!ResultModifier::Equals(other_p)) {
            return false;
        }
        auto &other = other_p.Cast<OrderModifier>();
        if (orders.size() != other.orders.size()) {
            return false;
        }
        for (idx_t i = 0; i < orders.size(); i++) {
            if (orders[i].type != other.orders[i].type) {
                return false;
            }
            if (!BaseExpression::Equals(*orders[i].expression, *other.orders[i].expression)) {
                return false;
            }
        }
        return true;
    }

    bool OrderModifier::Equals(const unique_ptr<OrderModifier> &left, const unique_ptr<OrderModifier> &right) {
        if (left.get() == right.get()) {
            return true;
        }
        if (!left || !right) {
            return false;
        }
        return left->Equals(*right);
    }

    unique_ptr<ResultModifier> OrderModifier::Copy() const {
        auto copy = make_uniq<OrderModifier>();
        for (auto &order: orders) {
            copy->orders.emplace_back(order.type, order.null_order, order.expression->Copy());
        }
        return copy;
    }

    string OrderByNode::ToString() const {
        auto str = expression->ToString();
        switch (type) {
            case OrderType::ASCENDING:
                str += " ASC";
                break;
            case OrderType::DESCENDING:
                str += " DESC";
                break;
            default:
                break;
        }

        switch (null_order) {
            case OrderByNullType::NULLS_FIRST:
                str += " NULLS FIRST";
                break;
            case OrderByNullType::NULLS_LAST:
                str += " NULLS LAST";
                break;
            default:
                break;
        }
        return str;
    }

    bool LimitPercentModifier::Equals(const ResultModifier &other_p) const {
        if (!ResultModifier::Equals(other_p)) {
            return false;
        }
        auto &other = other_p.Cast<LimitPercentModifier>();
        if (!ParsedExpression::Equals(limit, other.limit)) {
            return false;
        }
        if (!ParsedExpression::Equals(offset, other.offset)) {
            return false;
        }
        return true;
    }

    unique_ptr<ResultModifier> LimitPercentModifier::Copy() const {
        auto copy = make_uniq<LimitPercentModifier>();
        if (limit) {
            copy->limit = limit->Copy();
        }
        if (offset) {
            copy->offset = offset->Copy();
        }
        return copy;
    }
} // namespace goose
