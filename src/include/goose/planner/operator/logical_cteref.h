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
