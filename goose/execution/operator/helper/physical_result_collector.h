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

#include <goose/execution/physical_operator.h>
#include <goose/main/query_result.h>
#include <goose/common/enums/statement_type.h>

namespace goose {

class PreparedStatementData;
class ColumnDataCollection;

//! PhysicalResultCollector is an abstract class that is used to generate the final result of a query
class PhysicalResultCollector : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::RESULT_COLLECTOR;

public:
	PhysicalResultCollector(PhysicalPlan &physical_plan, PreparedStatementData &data);

	StatementType statement_type;
	StatementProperties properties;
	QueryResultMemoryType memory_type;
	PhysicalOperator &plan;
	vector<string> names;

public:
	static PhysicalOperator &GetResultCollector(ClientContext &context, PreparedStatementData &data);

public:
	//! The final method used to fetch the query result from this operator
	virtual unique_ptr<QueryResult> GetResult(GlobalSinkState &state) const = 0;

	bool IsSink() const override {
		return true;
	}

public:
	vector<const_reference<PhysicalOperator>> GetChildren() const override;
	void BuildPipelines(Pipeline &current, MetaPipeline &meta_pipeline) override;

	bool IsSource() const override {
		return true;
	}

public:
	//! Whether this is a streaming result collector
	virtual bool IsStreaming() const {
		return false;
	}

protected:
	unique_ptr<ColumnDataCollection> CreateCollection(ClientContext &context) const;
};

} // namespace goose
