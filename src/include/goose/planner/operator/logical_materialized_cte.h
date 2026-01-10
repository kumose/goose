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

namespace goose {

class LogicalMaterializedCTE : public LogicalCTE {
	explicit LogicalMaterializedCTE() : LogicalCTE(LogicalOperatorType::LOGICAL_MATERIALIZED_CTE) {
	}

public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_MATERIALIZED_CTE;

public:
	LogicalMaterializedCTE(string ctename_p, idx_t table_index, idx_t column_count, unique_ptr<LogicalOperator> cte,
	                       unique_ptr<LogicalOperator> child, CTEMaterialize materialize)
	    : LogicalCTE(std::move(ctename_p), table_index, column_count, std::move(cte), std::move(child),
	                 LogicalOperatorType::LOGICAL_MATERIALIZED_CTE),
	      materialize(materialize) {
	}

	CTEMaterialize materialize = CTEMaterialize::CTE_MATERIALIZE_ALWAYS;

public:
	InsertionOrderPreservingMap<string> ParamsToString() const override;
	vector<ColumnBinding> GetColumnBindings() override {
		return children[1]->GetColumnBindings();
	}

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

	vector<idx_t> GetTableIndex() const override;

protected:
	void ResolveTypes() override {
		types = children[1]->types;
	}
};
} // namespace goose
