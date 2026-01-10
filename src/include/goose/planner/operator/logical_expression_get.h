// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

#include <goose/planner/logical_operator.h>

namespace goose {

//! LogicalExpressionGet represents a scan operation over a set of to-be-executed expressions
class LogicalExpressionGet : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_EXPRESSION_GET;

public:
	LogicalExpressionGet(idx_t table_index, vector<LogicalType> types,
	                     vector<vector<unique_ptr<Expression>>> expressions)
	    : LogicalOperator(LogicalOperatorType::LOGICAL_EXPRESSION_GET), table_index(table_index),
	      expr_types(std::move(types)), expressions(std::move(expressions)) {
	}

	//! The table index in the current bind context
	idx_t table_index;
	//! The types of the expressions
	vector<LogicalType> expr_types;
	//! The set of expressions
	vector<vector<unique_ptr<Expression>>> expressions;

public:
	vector<ColumnBinding> GetColumnBindings() override {
		return GenerateColumnBindings(table_index, expr_types.size());
	}

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);
	idx_t EstimateCardinality(ClientContext &context) override {
		return expressions.size();
	}
	vector<idx_t> GetTableIndex() const override;
	string GetName() const override;

protected:
	void ResolveTypes() override {
		// types are resolved in the constructor
		this->types = expr_types;
	}
};
} // namespace goose
