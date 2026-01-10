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
#include <goose/parser/parsed_data/sample_options.h>

namespace goose {

//! LogicalSample represents a SAMPLE clause
class LogicalSample : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_SAMPLE;

public:
	LogicalSample(unique_ptr<SampleOptions> sample_options_p, unique_ptr<LogicalOperator> child);

	//! The sample options
	unique_ptr<SampleOptions> sample_options;

public:
	vector<ColumnBinding> GetColumnBindings() override;
	idx_t EstimateCardinality(ClientContext &context) override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

protected:
	void ResolveTypes() override;

private:
	LogicalSample();
};

} // namespace goose
