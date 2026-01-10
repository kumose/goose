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

//! LogicalEmptyResult returns an empty result. This is created by the optimizer if it can reason that certain parts of
//! the tree will always return an empty result.
class LogicalEmptyResult : public LogicalOperator {
	LogicalEmptyResult();

public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_EMPTY_RESULT;

public:
	explicit LogicalEmptyResult(unique_ptr<LogicalOperator> op);
	LogicalEmptyResult(vector<LogicalType> return_types, vector<ColumnBinding> bindings);

	//! The set of return types of the empty result
	vector<LogicalType> return_types;
	//! The columns that would be bound at this location (if the subtree was not optimized away)
	vector<ColumnBinding> bindings;

public:
	vector<ColumnBinding> GetColumnBindings() override {
		return bindings;
	}
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);
	idx_t EstimateCardinality(ClientContext &context) override {
		return 0;
	}

protected:
	void ResolveTypes() override {
		this->types = return_types;
	}
};
} // namespace goose
