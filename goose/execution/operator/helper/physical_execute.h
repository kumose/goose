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
#include <goose/main/prepared_statement_data.h>

namespace goose {

class PhysicalExecute : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::EXECUTE;

public:
	PhysicalExecute(PhysicalPlan &physical_plan, PhysicalOperator &plan);

	PhysicalOperator &plan;
	shared_ptr<PreparedStatementData> prepared;

public:
	vector<const_reference<PhysicalOperator>> GetChildren() const override;

public:
	void BuildPipelines(Pipeline &current, MetaPipeline &meta_pipeline) override;
};

} // namespace goose
