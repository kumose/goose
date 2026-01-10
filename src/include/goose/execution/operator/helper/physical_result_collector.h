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
