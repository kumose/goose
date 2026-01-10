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
#include <goose/common/enums/cte_materialize.h>

namespace goose {

//! LogicalCTERef represents a reference to a recursive CTE
class LogicalCTERef : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_CTE_REF;

public:
	LogicalCTERef(idx_t table_index, idx_t cte_index, vector<LogicalType> types, vector<string> colnames,
	              bool is_recurring = false)
	    : LogicalOperator(LogicalOperatorType::LOGICAL_CTE_REF), table_index(table_index), cte_index(cte_index),
	      correlated_columns(0), is_recurring(is_recurring) {
		D_ASSERT(!types.empty());
		chunk_types = std::move(types);
		bound_columns = std::move(colnames);
	}

	vector<string> bound_columns;
	//! The table index in the current bind context
	idx_t table_index;
	//! CTE index
	idx_t cte_index;
	//! The types of the chunk
	vector<LogicalType> chunk_types;
	//! Number of correlated columns
	idx_t correlated_columns;
	//! Does this operator read the recurring CTE table
	bool is_recurring = false;

public:
	InsertionOrderPreservingMap<string> ParamsToString() const override;
	vector<ColumnBinding> GetColumnBindings() override {
		return GenerateColumnBindings(table_index, chunk_types.size());
	}
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);
	vector<idx_t> GetTableIndex() const override;
	string GetName() const override;

protected:
	void ResolveTypes() override {
		// types are resolved in the constructor
		this->types = chunk_types;
	}
};
} // namespace goose
