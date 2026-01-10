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

#include <goose/function/cast/default_casts.h>

namespace goose {
struct MapCastInfo;
struct MapCastNode;
struct DBConfig;

typedef BoundCastInfo (*bind_cast_function_t)(BindCastInput &input, const LogicalType &source,
                                              const LogicalType &target);
typedef int64_t (*implicit_cast_cost_t)(const LogicalType &from, const LogicalType &to);

struct GetCastFunctionInput {
	explicit GetCastFunctionInput(optional_ptr<ClientContext> context = nullptr) : context(context) {
	}
	explicit GetCastFunctionInput(ClientContext &context) : context(&context) {
	}

	optional_ptr<ClientContext> context;
	optional_idx query_location;
};

struct BindCastFunction {
	BindCastFunction(bind_cast_function_t function, // NOLINT: allow implicit cast
	                 unique_ptr<BindCastInfo> info = nullptr);

	bind_cast_function_t function;
	unique_ptr<BindCastInfo> info;
};

class CastFunctionSet {
public:
	CastFunctionSet();
	explicit CastFunctionSet(DBConfig &config);

public:
	GOOSE_API static CastFunctionSet &Get(ClientContext &context);
	GOOSE_API static CastFunctionSet &Get(DatabaseInstance &db);

	//! Returns a cast function (from source -> target)
	//! Note that this always returns a function - since a cast is ALWAYS possible if the value is NULL
	GOOSE_API BoundCastInfo GetCastFunction(const LogicalType &source, const LogicalType &target,
	                                         GetCastFunctionInput &input);
	//! Returns the implicit cast cost of casting from source -> target
	//! -1 means an implicit cast is not possible
	GOOSE_API int64_t ImplicitCastCost(optional_ptr<ClientContext> context, const LogicalType &source,
	                                    const LogicalType &target);
	GOOSE_API static int64_t ImplicitCastCost(ClientContext &context, const LogicalType &source,
	                                           const LogicalType &target);
	GOOSE_API static int64_t ImplicitCastCost(DatabaseInstance &db, const LogicalType &source,
	                                           const LogicalType &target);
	//! Register a new cast function from source to target
	GOOSE_API void RegisterCastFunction(const LogicalType &source, const LogicalType &target, BoundCastInfo function,
	                                     int64_t implicit_cast_cost = -1);
	GOOSE_API void RegisterCastFunction(const LogicalType &source, const LogicalType &target,
	                                     bind_cast_function_t bind, int64_t implicit_cast_cost = -1);

private:
	optional_ptr<DBConfig> config;
	vector<BindCastFunction> bind_functions;
	//! If any custom cast functions have been defined using RegisterCastFunction, this holds the map
	optional_ptr<MapCastInfo> map_info;

private:
	void RegisterCastFunction(const LogicalType &source, const LogicalType &target, MapCastNode node);
};

} // namespace goose
