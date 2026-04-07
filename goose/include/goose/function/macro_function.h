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

#include <goose/function/function.h>
#include <goose/main/client_context.h>
#include <goose/parser/expression/constant_expression.h>
#include <goose/parser/query_node.h>
#include <goose/planner/binder.h>
#include <goose/planner/expression_binder.h>

namespace goose {

class ScalarMacroFunction;

enum class MacroType : uint8_t { VOID_MACRO = 0, TABLE_MACRO = 1, SCALAR_MACRO = 2 };

struct MacroBindResult {
	explicit MacroBindResult(string error_p) : error(std::move(error_p)) {
	}
	explicit MacroBindResult(idx_t function_idx) : function_idx(function_idx) {
	}

	optional_idx function_idx;
	string error;
};

class MacroFunction {
public:
	explicit MacroFunction(MacroType type);

	//! The type
	MacroType type;
	//! The parameters (ColumnRefExpression)
	vector<unique_ptr<ParsedExpression>> parameters;
	//! The default values of the parameters
	InsertionOrderPreservingMap<unique_ptr<ParsedExpression>> default_parameters;
	//! The types of the parameters
	vector<LogicalType> types;

public:
	virtual ~MacroFunction() {
	}

	void CopyProperties(MacroFunction &other) const;

	virtual unique_ptr<MacroFunction> Copy() const = 0;

	vector<unique_ptr<ParsedExpression>> GetPositionalParametersForSerialization(Serializer &serializer) const;
	void FinalizeDeserialization();

	static MacroBindResult BindMacroFunction(Binder &binder, const vector<unique_ptr<MacroFunction>> &macro_functions,
	                                         const string &name, FunctionExpression &function_expr,
	                                         vector<unique_ptr<ParsedExpression>> &positional_arguments,
	                                         InsertionOrderPreservingMap<unique_ptr<ParsedExpression>> &named_arguments,
	                                         idx_t depth);
	static unique_ptr<DummyBinding>
	CreateDummyBinding(const MacroFunction &macro_def, const string &name,
	                   vector<unique_ptr<ParsedExpression>> &positional_arguments,
	                   InsertionOrderPreservingMap<unique_ptr<ParsedExpression>> &named_arguments);

	virtual string ToSQL() const;

	virtual void Serialize(Serializer &serializer) const;
	static unique_ptr<MacroFunction> Deserialize(Deserializer &deserializer);

public:
	template <class TARGET>
	TARGET &Cast() {
		if (type != TARGET::TYPE) {
			throw InternalException("Failed to cast macro to type - macro type mismatch");
		}
		return reinterpret_cast<TARGET &>(*this);
	}

	template <class TARGET>
	const TARGET &Cast() const {
		if (type != TARGET::TYPE) {
			throw InternalException("Failed to cast macro to type - macro type mismatch");
		}
		return reinterpret_cast<const TARGET &>(*this);
	}
};

} // namespace goose
