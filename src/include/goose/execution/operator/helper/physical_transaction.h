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
#include <goose/parser/parsed_data/transaction_info.h>

namespace goose {

//! PhysicalTransaction represents a transaction operator (e.g. BEGIN or COMMIT)
class PhysicalTransaction : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::TRANSACTION;

public:
	explicit PhysicalTransaction(PhysicalPlan &physical_plan, unique_ptr<TransactionInfo> info,
	                             idx_t estimated_cardinality)
	    : PhysicalOperator(physical_plan, PhysicalOperatorType::TRANSACTION, {LogicalType::BOOLEAN},
	                       estimated_cardinality),
	      info(std::move(info)) {
	}

	unique_ptr<TransactionInfo> info;

public:
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}
};

} // namespace goose
