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

#include <goose/common/common.h>
#include <goose/common/enums/quantile_enum.h>
#include "core_functions/aggregate/holistic_functions.h"

namespace goose {

//	Avoid using naked Values in inner loops...
struct QuantileValue {
	explicit QuantileValue(const Value &v) : val(v), dbl(v.GetValue<double>()) {
		const auto &type = val.type();
		switch (type.id()) {
		case LogicalTypeId::DECIMAL: {
			integral = IntegralValue::Get(v);
			scaling = Hugeint::POWERS_OF_TEN[DecimalType::GetScale(type)];
			break;
		}
		default:
			break;
		}
	}

	Value val;

	//	DOUBLE
	double dbl;

	//	DECIMAL
	hugeint_t integral;
	hugeint_t scaling;

	inline bool operator==(const QuantileValue &other) const {
		return val == other.val;
	}
};

struct QuantileBindData : public FunctionData {
	QuantileBindData();
	explicit QuantileBindData(const Value &quantile_p);
	explicit QuantileBindData(const vector<Value> &quantiles_p);
	QuantileBindData(const QuantileBindData &other);

	unique_ptr<FunctionData> Copy() const override;
	bool Equals(const FunctionData &other_p) const override;

	static void Serialize(Serializer &serializer, const optional_ptr<FunctionData> bind_data_p,
	                      const AggregateFunction &function);

	static unique_ptr<FunctionData> Deserialize(Deserializer &deserializer, AggregateFunction &function);

	vector<QuantileValue> quantiles;
	vector<idx_t> order;
	bool desc;
};

} // namespace goose
