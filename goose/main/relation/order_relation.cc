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


#include <goose/main/relation/order_relation.h>
#include <goose/main/client_context.h>
#include <goose/parser/query_node.h>
#include <goose/parser/query_node/select_node.h>
#include <goose/parser/expression/star_expression.h>

namespace goose {
    OrderRelation::OrderRelation(shared_ptr<Relation> child_p, vector<OrderByNode> orders)
        : Relation(child_p->context, RelationType::ORDER_RELATION), orders(std::move(orders)),
          child(std::move(child_p)) {
        D_ASSERT(child.get() != this);
        // bind the expressions
        TryBindRelation(columns);
    }

    unique_ptr<QueryNode> OrderRelation::GetQueryNode() {
        auto select = make_uniq<SelectNode>();
        select->from_table = child->GetTableRef();
        select->select_list.push_back(make_uniq<StarExpression>());
        auto order_node = make_uniq<OrderModifier>();
        for (idx_t i = 0; i < orders.size(); i++) {
            order_node->orders.emplace_back(orders[i].type, orders[i].null_order, orders[i].expression->Copy());
        }
        select->modifiers.push_back(std::move(order_node));
        return select;
    }

    string OrderRelation::GetAlias() {
        return child->GetAlias();
    }

    const vector<ColumnDefinition> &OrderRelation::Columns() {
        return columns;
    }

    string OrderRelation::ToString(idx_t depth) {
        string str = RenderWhitespace(depth) + "Order [";
        for (idx_t i = 0; i < orders.size(); i++) {
            if (i != 0) {
                str += ", ";
            }
            str += orders[i].expression->ToString() + (orders[i].type == OrderType::ASCENDING ? " ASC" : " DESC");
        }
        str += "]\n";
        return str + child->ToString(depth + 1);
    }
} // namespace goose
