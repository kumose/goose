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
#include <goose/planner/bound_result_modifier.h>

namespace goose {

//! LogicalDistinct filters duplicate entries from its child operator
class LogicalDistinct : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_DISTINCT;

public:
	explicit LogicalDistinct(DistinctType distinct_type);
	explicit LogicalDistinct(vector<unique_ptr<Expression>> targets, DistinctType distinct_type);

	//! Whether or not this is a DISTINCT or DISTINCT ON
	DistinctType distinct_type;
	//! The set of distinct targets
	vector<unique_ptr<Expression>> distinct_targets;
	//! The order by modifier (optional, only for distinct on)
	unique_ptr<BoundOrderModifier> order_by;

public:
	InsertionOrderPreservingMap<string> ParamsToString() const override;

	vector<ColumnBinding> GetColumnBindings() override {
		return children[0]->GetColumnBindings();
	}

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

protected:
	void ResolveTypes() override;
};
} // namespace goose
