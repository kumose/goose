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
