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

#include <goose/function/function.h>
#include <goose/parser/parsed_data/pragma_info.h>
#include <goose/common/types-import.h>

namespace goose {
class ClientContext;

//! Return a substitute query to execute instead of this pragma statement
typedef string (*pragma_query_t)(ClientContext &context, const FunctionParameters &parameters);
//! Execute the main pragma function
typedef void (*pragma_function_t)(ClientContext &context, const FunctionParameters &parameters);

//! Pragma functions are invoked by calling PRAGMA x
//! Pragma functions come in three types:
//! * Call: function call, e.g. PRAGMA table_info('tbl')
//!   -> call statements can take multiple parameters
//! * Statement: statement without parameters, e.g. PRAGMA show_tables
//!   -> this is similar to a call pragma but without parameters
//! Pragma functions can either return a new query to execute (pragma_query_t)
//! or they can
class PragmaFunction : public SimpleNamedParameterFunction { // NOLINT: work-around bug in clang-tidy
public:
	// Call
	GOOSE_API static PragmaFunction PragmaCall(const string &name, pragma_query_t query, vector<LogicalType> arguments,
	                                            LogicalType varargs = LogicalType::INVALID);
	GOOSE_API static PragmaFunction PragmaCall(const string &name, pragma_function_t function,
	                                            vector<LogicalType> arguments,
	                                            LogicalType varargs = LogicalType::INVALID);
	// Statement
	GOOSE_API static PragmaFunction PragmaStatement(const string &name, pragma_query_t query);
	GOOSE_API static PragmaFunction PragmaStatement(const string &name, pragma_function_t function);

	GOOSE_API string ToString() const override;

public:
	PragmaType type;

	pragma_query_t query;
	pragma_function_t function;
	named_parameter_type_map_t named_parameters;

private:
	PragmaFunction(string name, PragmaType pragma_type, pragma_query_t query, pragma_function_t function,
	               vector<LogicalType> arguments, LogicalType varargs);
};

} // namespace goose
