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


#include <goose/planner/filter/null_filter.h>
#include <goose/planner/expression/bound_operator_expression.h>
#include <goose/storage/statistics/base_statistics.h>

namespace goose {
    IsNullFilter::IsNullFilter() : TableFilter(TableFilterType::IS_NULL) {
    }

    FilterPropagateResult IsNullFilter::CheckStatistics(BaseStatistics &stats) const {
        if (!stats.CanHaveNull()) {
            // no null values are possible: always false
            return FilterPropagateResult::FILTER_ALWAYS_FALSE;
        }
        if (!stats.CanHaveNoNull()) {
            // no non-null values are possible: always true
            return FilterPropagateResult::FILTER_ALWAYS_TRUE;
        }
        return FilterPropagateResult::NO_PRUNING_POSSIBLE;
    }

    string IsNullFilter::ToString(const string &column_name) const {
        return column_name + " IS NULL";
    }

    unique_ptr<TableFilter> IsNullFilter::Copy() const {
        return make_uniq<IsNullFilter>();
    }

    unique_ptr<Expression> IsNullFilter::ToExpression(const Expression &column) const {
        auto result = make_uniq<BoundOperatorExpression>(ExpressionType::OPERATOR_IS_NULL, LogicalType::BOOLEAN);
        result->children.push_back(column.Copy());
        return result;
    }

    IsNotNullFilter::IsNotNullFilter() : TableFilter(TableFilterType::IS_NOT_NULL) {
    }

    FilterPropagateResult IsNotNullFilter::CheckStatistics(BaseStatistics &stats) const {
        if (!stats.CanHaveNoNull()) {
            // no non-null values are possible: always false
            return FilterPropagateResult::FILTER_ALWAYS_FALSE;
        }
        if (!stats.CanHaveNull()) {
            // no null values are possible: always true
            return FilterPropagateResult::FILTER_ALWAYS_TRUE;
        }
        return FilterPropagateResult::NO_PRUNING_POSSIBLE;
    }

    string IsNotNullFilter::ToString(const string &column_name) const {
        return column_name + " IS NOT NULL";
    }

    unique_ptr<TableFilter> IsNotNullFilter::Copy() const {
        return make_uniq<IsNotNullFilter>();
    }

    unique_ptr<Expression> IsNotNullFilter::ToExpression(const Expression &column) const {
        auto result = make_uniq<BoundOperatorExpression>(ExpressionType::OPERATOR_IS_NOT_NULL, LogicalType::BOOLEAN);
        result->children.push_back(column.Copy());
        return result;
    }
} // namespace goose
