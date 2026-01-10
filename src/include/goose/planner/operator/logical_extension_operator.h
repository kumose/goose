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

#include <goose/planner/logical_operator.h>
#include <goose/planner/operator_extension.h>

namespace goose {

class ColumnBindingResolver;

struct LogicalExtensionOperator : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_EXTENSION_OPERATOR;

public:
	LogicalExtensionOperator() : LogicalOperator(LogicalOperatorType::LOGICAL_EXTENSION_OPERATOR) {
	}
	explicit LogicalExtensionOperator(vector<unique_ptr<Expression>> expressions)
	    : LogicalOperator(LogicalOperatorType::LOGICAL_EXTENSION_OPERATOR, std::move(expressions)) {
	}

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

	virtual PhysicalOperator &CreatePlan(ClientContext &context, PhysicalPlanGenerator &planner) = 0;

	virtual void ResolveColumnBindings(ColumnBindingResolver &res, vector<ColumnBinding> &bindings);
	virtual string GetExtensionName() const;
};
} // namespace goose
