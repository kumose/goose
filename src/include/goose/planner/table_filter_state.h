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

#include <goose/planner/table_filter.h>
#include <goose/common/types/selection_vector.h>
#include <goose/execution/expression_executor.h>

namespace goose {

//! Thread-local state for executing a table filter
struct TableFilterState {
public:
	virtual ~TableFilterState() = default;

public:
	static unique_ptr<TableFilterState> Initialize(ClientContext &context, const TableFilter &filter);

public:
	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}
};

struct ConjunctionAndFilterState : public TableFilterState {
public:
	vector<unique_ptr<TableFilterState>> child_states;
};

struct ConjunctionOrFilterState : public TableFilterState {
public:
	vector<unique_ptr<TableFilterState>> child_states;
};

struct ExpressionFilterState : public TableFilterState {
public:
	ExpressionFilterState(ClientContext &context, const Expression &expression);

	ExpressionExecutor executor;
};

struct BFTableFilterState final : public TableFilterState {
	idx_t current_capacity;
	Vector hashes_v;
	Vector found_v;
	Vector keys_sliced_v;
	SelectionVector bf_sel;

	explicit BFTableFilterState(const LogicalType &key_logical_type)
	    : current_capacity(STANDARD_VECTOR_SIZE), hashes_v(LogicalType::HASH), found_v(LogicalType::UBIGINT),
	      keys_sliced_v(key_logical_type), bf_sel(STANDARD_VECTOR_SIZE) {
	}
};

} // namespace goose
