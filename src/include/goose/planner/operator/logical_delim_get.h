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

namespace goose {

//! LogicalDelimGet represents a duplicate eliminated scan belonging to a DelimJoin
class LogicalDelimGet : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_DELIM_GET;

public:
	LogicalDelimGet(idx_t table_index, vector<LogicalType> types)
	    : LogicalOperator(LogicalOperatorType::LOGICAL_DELIM_GET), table_index(table_index) {
		D_ASSERT(!types.empty());
		chunk_types = std::move(types);
	}

	//! The table index in the current bind context
	idx_t table_index;
	//! The types of the chunk
	vector<LogicalType> chunk_types;

public:
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
