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

#include <goose/common/enums/statement_type.h>
#include <goose/common/types/value.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/winapi.h>
#include <goose/planner/expression/bound_parameter_data.h>
#include <goose/planner/bound_parameter_map.h>

namespace goose {
class CatalogEntry;
class ClientContext;
class PhysicalPlan;
class SQLStatement;

class PreparedStatementData {
public:
	GOOSE_API explicit PreparedStatementData(StatementType type);
	GOOSE_API ~PreparedStatementData();

	StatementType statement_type;
	//! The unbound SQL statement that was prepared
	unique_ptr<SQLStatement> unbound_statement;

	//! The physical plan.
	unique_ptr<PhysicalPlan> physical_plan;

	//! The result names of the transaction
	vector<string> names;
	//! The result types of the transaction
	vector<LogicalType> types;

	//! The statement properties
	StatementProperties properties;

	//! The map of parameter index to the actual value entry
	bound_parameter_map_t value_map;
	//! Whether we are creating a streaming result or not
	QueryResultOutputType output_type;
	//! Whether we are creating a buffer-managed result or not
	QueryResultMemoryType memory_type;

public:
	void CheckParameterCount(idx_t parameter_count);
	//! Whether or not the prepared statement data requires the query to rebound for the given parameters
	bool RequireRebind(ClientContext &context, optional_ptr<case_insensitive_map_t<BoundParameterData>> values);
	//! Bind a set of values to the prepared statement data
	GOOSE_API void Bind(case_insensitive_map_t<BoundParameterData> values);
	//! Get the expected SQL Type of the bound parameter
	GOOSE_API LogicalType GetType(const string &identifier);
	//! Try to get the expected SQL Type of the bound parameter
	GOOSE_API bool TryGetType(const string &identifier, LogicalType &result);
};

} // namespace goose
