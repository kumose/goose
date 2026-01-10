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

#include <goose/common/named_parameter_map.h>
#include <goose/common/types/data_chunk.h>
#include <goose/common/types-import.h>
#include <goose/main/external_dependencies.h>
#include <goose/parser/column_definition.h>
#include <goose/common/enums/function_errors.h>

namespace goose {
class CatalogEntry;
class Catalog;
class ClientContext;
class Expression;
class ExpressionExecutor;
class Transaction;

class AggregateFunction;
class AggregateFunctionSet;
class CopyFunction;
class PragmaFunction;
class PragmaFunctionSet;
class ScalarFunctionSet;
class ScalarFunction;
class TableFunctionSet;
class TableFunction;
class SimpleFunction;

struct PragmaInfo;

//! The default null handling is NULL in, NULL out
enum class FunctionNullHandling : uint8_t { DEFAULT_NULL_HANDLING = 0, SPECIAL_HANDLING = 1 };
//! The stability of the function, used by the optimizer
//! CONSISTENT              -> this function always returns the same result when given the same input, no variance
//! CONSISTENT_WITHIN_QUERY -> this function returns the same result WITHIN the same query/transaction
//!                            but the result might change across queries (e.g. NOW(), CURRENT_TIME)
//! VOLATILE                -> the result of this function might change per row (e.g. RANDOM())
enum class FunctionStability : uint8_t { CONSISTENT = 0, VOLATILE = 1, CONSISTENT_WITHIN_QUERY = 2 };

//! How to handle collations
//! PROPAGATE_COLLATIONS        -> this function combines collation from its inputs and emits them again (default)
//! PUSH_COMBINABLE_COLLATIONS  -> combinable collations are executed for the input arguments
//! IGNORE_COLLATIONS           -> collations are completely ignored by the function
enum class FunctionCollationHandling : uint8_t {
	PROPAGATE_COLLATIONS = 0,
	PUSH_COMBINABLE_COLLATIONS = 1,
	IGNORE_COLLATIONS = 2
};

struct FunctionData {
	GOOSE_API virtual ~FunctionData();

	GOOSE_API virtual unique_ptr<FunctionData> Copy() const = 0;
	GOOSE_API virtual bool Equals(const FunctionData &other) const = 0;
	GOOSE_API static bool Equals(const FunctionData *left, const FunctionData *right);
	GOOSE_API virtual bool SupportStatementCache() const;

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
	// FIXME: this function should be removed in the future
	template <class TARGET>
	TARGET &CastNoConst() const {
		return const_cast<TARGET &>(Cast<TARGET>()); // NOLINT: FIXME
	}
};

struct TableFunctionData : public FunctionData {
	// used to pass on projections to table functions that support them. NB, can contain COLUMN_IDENTIFIER_ROW_ID
	vector<idx_t> column_ids;

	GOOSE_API ~TableFunctionData() override;

	GOOSE_API unique_ptr<FunctionData> Copy() const override;
	GOOSE_API bool Equals(const FunctionData &other) const override;
};

struct FunctionParameters {
	vector<Value> values;
	named_parameter_map_t named_parameters;
};

//! Function is the base class used for any type of function (scalar, aggregate or simple function)
class Function {
public:
	GOOSE_API explicit Function(string name);
	GOOSE_API virtual ~Function();

	//! The name of the function
	string name;
	//! Additional Information to specify function from it's name
	string extra_info;

	// Optional catalog name of the function
	string catalog_name;

	// Optional schema name of the function
	string schema_name;

public:
	//! Returns the formatted string name(arg1, arg2, ...)
	GOOSE_API static string CallToString(const string &catalog_name, const string &schema_name, const string &name,
	                                      const vector<LogicalType> &arguments,
	                                      const LogicalType &varargs = LogicalType::INVALID);
	//! Returns the formatted string name(arg1, arg2..) -> return_type
	GOOSE_API static string CallToString(const string &catalog_name, const string &schema_name, const string &name,
	                                      const vector<LogicalType> &arguments, const LogicalType &varargs,
	                                      const LogicalType &return_type);
	//! Returns the formatted string name(arg1, arg2.., np1=a, np2=b, ...)
	GOOSE_API static string CallToString(const string &catalog_name, const string &schema_name, const string &name,
	                                      const vector<LogicalType> &arguments,
	                                      const named_parameter_type_map_t &named_parameters);

	//! Used in the bind to erase an argument from a function
	GOOSE_API static void EraseArgument(SimpleFunction &bound_function, vector<unique_ptr<Expression>> &arguments,
	                                     idx_t argument_index);
};

class SimpleFunction : public Function {
public:
	GOOSE_API SimpleFunction(string name, vector<LogicalType> arguments,
	                          LogicalType varargs = LogicalType(LogicalTypeId::INVALID));
	GOOSE_API ~SimpleFunction() override;

	//! The set of arguments of the function
	vector<LogicalType> arguments;
	//! The set of original arguments of the function - only set if Function::EraseArgument is called
	//! Used for (de)serialization purposes
	vector<LogicalType> original_arguments;
	//! The type of varargs to support, or LogicalTypeId::INVALID if the function does not accept variable length
	//! arguments
	LogicalType varargs;

public:
	GOOSE_API virtual string ToString() const;

	GOOSE_API bool HasVarArgs() const;
};

class SimpleNamedParameterFunction : public SimpleFunction {
public:
	GOOSE_API SimpleNamedParameterFunction(string name, vector<LogicalType> arguments,
	                                        LogicalType varargs = LogicalType(LogicalTypeId::INVALID));
	GOOSE_API ~SimpleNamedParameterFunction() override;

	//! The named parameters of the function
	named_parameter_type_map_t named_parameters;

public:
	GOOSE_API string ToString() const override;
	GOOSE_API bool HasNamedParameters() const;
};

class BaseScalarFunction : public SimpleFunction {
public:
	GOOSE_API BaseScalarFunction(string name, vector<LogicalType> arguments, LogicalType return_type,
	                              FunctionStability stability,
	                              LogicalType varargs = LogicalType(LogicalTypeId::INVALID),
	                              FunctionNullHandling null_handling = FunctionNullHandling::DEFAULT_NULL_HANDLING,
	                              FunctionErrors errors = FunctionErrors::CANNOT_ERROR);
	GOOSE_API ~BaseScalarFunction() override;

public:
	void SetReturnType(LogicalType return_type_p) {
		return_type = std::move(return_type_p);
	}
	const LogicalType &GetReturnType() const {
		return return_type;
	}
	LogicalType &GetReturnType() {
		return return_type;
	}

	FunctionStability GetStability() const {
		return stability;
	}
	void SetStability(FunctionStability stability_p) {
		stability = stability_p;
	}

	FunctionNullHandling GetNullHandling() const {
		return null_handling;
	}
	void SetNullHandling(FunctionNullHandling null_handling_p) {
		null_handling = null_handling_p;
	}

	FunctionErrors GetErrorMode() const {
		return errors;
	}
	void SetErrorMode(FunctionErrors errors_p) {
		errors = errors_p;
	}

	//! Set this functions error-mode as fallible (can throw runtime errors)
	void SetFallible() {
		errors = FunctionErrors::CAN_THROW_RUNTIME_ERROR;
	}
	//! Set this functions stability as volatile (can not be cached per row)
	void SetVolatile() {
		stability = FunctionStability::VOLATILE;
	}

	void SetCollationHandling(FunctionCollationHandling collation_handling_p) {
		collation_handling = collation_handling_p;
	}
	FunctionCollationHandling GetCollationHandling() const {
		return collation_handling;
	}

public:
	//! Return type of the function
	LogicalType return_type;
	//! The stability of the function (see FunctionStability enum for more info)
	FunctionStability stability;
	//! How this function handles NULL values
	FunctionNullHandling null_handling;
	//! Whether or not this function can throw an error
	FunctionErrors errors;
	//! Collation handling of the function
	FunctionCollationHandling collation_handling;

	static BaseScalarFunction SetReturnsError(BaseScalarFunction &function) {
		function.errors = FunctionErrors::CAN_THROW_RUNTIME_ERROR;
		return function;
	}

public:
	GOOSE_API hash_t Hash() const;

	GOOSE_API string ToString() const override;
};

} // namespace goose
