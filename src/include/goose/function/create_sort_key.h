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

#include <goose/function/function_set.h>

namespace goose {

struct OrderModifiers {
	OrderModifiers(OrderType order_type, OrderByNullType null_type) : order_type(order_type), null_type(null_type) {
	}

	OrderType order_type;
	OrderByNullType null_type;

	bool operator==(const OrderModifiers &other) const {
		return order_type == other.order_type && null_type == other.null_type;
	}

	static OrderModifiers Parse(const string &val) {
		auto lcase = StringUtil::Replace(StringUtil::Lower(val), "_", " ");
		OrderType order_type;
		if (StringUtil::StartsWith(lcase, "asc")) {
			order_type = OrderType::ASCENDING;
		} else if (StringUtil::StartsWith(lcase, "desc")) {
			order_type = OrderType::DESCENDING;
		} else {
			throw BinderException("create_sort_key modifier must start with either ASC or DESC");
		}
		OrderByNullType null_type;
		if (StringUtil::EndsWith(lcase, "nulls first")) {
			null_type = OrderByNullType::NULLS_FIRST;
		} else if (StringUtil::EndsWith(lcase, "nulls last")) {
			null_type = OrderByNullType::NULLS_LAST;
		} else {
			throw BinderException("create_sort_key modifier must end with either NULLS FIRST or NULLS LAST");
		}
		return OrderModifiers(order_type, null_type);
	}
};

struct CreateSortKeyHelpers {
	static void CreateSortKey(DataChunk &input, const vector<OrderModifiers> &modifiers, Vector &result);
	static void CreateSortKey(Vector &input, idx_t input_count, OrderModifiers modifiers, Vector &result);
	static idx_t DecodeSortKey(string_t sort_key, Vector &result, idx_t result_idx, OrderModifiers modifiers);
	static void DecodeSortKey(string_t sort_key, DataChunk &result, idx_t result_idx,
	                          const vector<OrderModifiers> &modifiers);
	static void CreateSortKeyWithValidity(Vector &input, Vector &result, const OrderModifiers &modifiers,
	                                      const idx_t count);
};

//! We don't add this function to the catalog, for internal use only
//! Therefore, it's not defined in src/function/scalar/generic/functions.json
struct DecodeSortKeyFun {
	static constexpr const char *Name = "decode_sort_key";
	static constexpr const char *Parameters = "parameters...";
	static constexpr const char *Description =
	    "Decodes a sort key created with create_sort_key into a STRUCT based on a set of sort qualifiers";
	static constexpr const char *Example = "decode_sort_key(sort_key, 'A INTEGER', 'DESC')";
	static constexpr const char *Categories = "";

	static ScalarFunction GetFunction();
};

} // namespace goose
