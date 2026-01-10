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

#include <goose/planner/bound_query_node.h>
#include <goose/planner/logical_operator.h>

namespace goose {
struct DynamicFilterData;

//! LogicalTopN represents a comibination of ORDER BY and LIMIT clause, using Min/Max Heap
class LogicalTopN : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_TOP_N;

public:
	LogicalTopN(vector<BoundOrderByNode> orders, idx_t limit, idx_t offset);
	~LogicalTopN() override;

	vector<BoundOrderByNode> orders;
	//! The maximum amount of elements to emit
	idx_t limit;
	//! The offset from the start to begin emitting elements
	idx_t offset;
	//! Dynamic table filter (if any)
	shared_ptr<DynamicFilterData> dynamic_filter;

public:
	vector<ColumnBinding> GetColumnBindings() override {
		return children[0]->GetColumnBindings();
	}

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

	idx_t EstimateCardinality(ClientContext &context) override;

protected:
	void ResolveTypes() override {
		types = children[0]->types;
	}
};
} // namespace goose
