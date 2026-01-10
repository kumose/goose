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

#include <goose/planner/operator/logical_cte.h>
#include <goose/planner/logical_operator.h>
#include <goose/planner/binder.h>

namespace goose {

class LogicalRecursiveCTE : public LogicalCTE {
	LogicalRecursiveCTE() : LogicalCTE(LogicalOperatorType::LOGICAL_RECURSIVE_CTE) {
	}

public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_RECURSIVE_CTE;

public:
	LogicalRecursiveCTE(string ctename_p, idx_t table_index, idx_t column_count, bool union_all,
	                    vector<unique_ptr<Expression>> key_targets, unique_ptr<LogicalOperator> top,
	                    unique_ptr<LogicalOperator> bottom)
	    : LogicalCTE(std::move(ctename_p), table_index, column_count, std::move(top), std::move(bottom),
	                 LogicalOperatorType::LOGICAL_RECURSIVE_CTE),
	      union_all(union_all), key_targets(std::move(key_targets)) {
	}

	bool union_all;
	// Flag if recurring table is referenced, if not we do not copy ht into ColumnDataCollection
	bool ref_recurring;
	vector<unique_ptr<Expression>> key_targets;

public:
	InsertionOrderPreservingMap<string> ParamsToString() const override;

	vector<ColumnBinding> GetColumnBindings() override {
		return GenerateColumnBindings(table_index, column_count);
	}

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

	vector<idx_t> GetTableIndex() const override;
	string GetName() const override;

protected:
	void ResolveTypes() override {
		types = children[0]->types;
	}
};
} // namespace goose
