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

#include <goose/execution/physical_operator.h>
#include <goose/planner/expression.h>
#include <goose/planner/bound_constraint.h>

namespace goose {
class DataTable;

//! Physically update data in a table
class PhysicalUpdate : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::UPDATE;

public:
	PhysicalUpdate(PhysicalPlan &physical_plan, vector<LogicalType> types, TableCatalogEntry &tableref,
	               DataTable &table, vector<PhysicalIndex> columns, vector<unique_ptr<Expression>> expressions,
	               vector<unique_ptr<Expression>> bound_defaults, vector<unique_ptr<BoundConstraint>> bound_constraints,
	               idx_t estimated_cardinality, bool return_chunk);

	TableCatalogEntry &tableref;
	DataTable &table;
	vector<PhysicalIndex> columns;
	vector<unique_ptr<Expression>> expressions;
	vector<unique_ptr<Expression>> bound_defaults;
	vector<unique_ptr<BoundConstraint>> bound_constraints;
	bool update_is_del_and_insert;
	//! If the returning statement is present, return the whole chunk
	bool return_chunk;
	//! Set to true, if we are updating an index column.
	bool index_update;

public:
	// Source interface
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}

public:
	// Sink interface
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;
	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;

	bool IsSink() const override {
		return true;
	}
	bool ParallelSink() const override {
		return true;
	}
};

} // namespace goose
