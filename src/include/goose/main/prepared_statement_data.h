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
