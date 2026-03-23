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

#include <goose/parser/parsed_data/create_secret_info.h>
#include <goose/planner/logical_operator.h>

namespace goose {

//! LogicalCreateSecret represents a simple logical operator that only passes on the parse info
class LogicalCreateSecret : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_CREATE_SECRET;

public:
	explicit LogicalCreateSecret(CreateSecretInput secret_input_p)
	    : LogicalOperator(LogicalOperatorType::LOGICAL_CREATE_SECRET), secret_input(std::move(secret_input_p)) {
	}

	CreateSecretInput secret_input;

public:
	idx_t EstimateCardinality(ClientContext &context) override {
		return 1;
	};

	//! Skips the serialization check in VerifyPlan
	bool SupportSerialization() const override {
		return false;
	}

protected:
	void ResolveTypes() override {
		types.emplace_back(LogicalType::BOOLEAN);
	}
};
} // namespace goose
