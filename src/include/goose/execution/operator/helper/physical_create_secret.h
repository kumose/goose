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
#include <goose/main/secret/secret.h>

namespace goose {

//! PhysicalCreateSecret represents the CREATE SECRET operator
class PhysicalCreateSecret : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::CREATE_SECRET;

public:
	PhysicalCreateSecret(PhysicalPlan &physical_plan, CreateSecretInput input_p, idx_t estimated_cardinality)
	    : PhysicalOperator(physical_plan, PhysicalOperatorType::CREATE_SECRET, {LogicalType::BOOLEAN},
	                       estimated_cardinality),
	      create_input(std::move(input_p)) {
	}

	CreateSecretInput create_input;

public:
	// Source interface
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}
};

} // namespace goose
