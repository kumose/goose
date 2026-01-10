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

#include <goose/goose-c.h>
#include <goose/goose.h>
#include <goose/common/types.h>
#include <goose/common/types/data_chunk.h>
#include <goose/main/appender.h>
#include <goose/common/case_insensitive_map.h>
#include <goose/main/client_context.h>
#include <goose/planner/expression/bound_parameter_data.h>
#include <goose/main/db_instance_cache.h>

#include <cstring>
#include <cassert>

#ifdef _WIN32
#ifndef strdup
#define strdup _strdup
#endif
#endif

namespace goose {

struct DBInstanceCacheWrapper {
	unique_ptr<DBInstanceCache> instance_cache;
};

struct DatabaseWrapper {
	shared_ptr<Goose> database;
};

struct CClientContextWrapper {
	explicit CClientContextWrapper(ClientContext &context) : context(context) {};
	ClientContext &context;
};

struct CClientArrowOptionsWrapper {
	explicit CClientArrowOptionsWrapper(ClientProperties &properties) : properties(properties) {};
	ClientProperties properties;
};

struct PreparedStatementWrapper {
	//! Map of name -> values
	case_insensitive_map_t<BoundParameterData> values;
	unique_ptr<PreparedStatement> statement;
	bool success = true;
	ErrorData error_data;
};

struct ExtractStatementsWrapper {
	vector<unique_ptr<SQLStatement>> statements;
	string error;
};

struct PendingStatementWrapper {
	unique_ptr<PendingQueryResult> statement;
	bool allow_streaming;
};

struct ArrowResultWrapper {
	unique_ptr<MaterializedQueryResult> result;
	unique_ptr<DataChunk> current_chunk;
};

struct AppenderWrapper {
	unique_ptr<BaseAppender> appender;
	ErrorData error_data;
};

struct TableDescriptionWrapper {
	unique_ptr<TableDescription> description;
	string error;
};

struct ErrorDataWrapper {
	ErrorData error_data;
};

struct ExpressionWrapper {
	unique_ptr<Expression> expr;
};

enum class CAPIResultSetType : uint8_t {
	CAPI_RESULT_TYPE_NONE = 0,
	CAPI_RESULT_TYPE_MATERIALIZED,
	CAPI_RESULT_TYPE_STREAMING,
	CAPI_RESULT_TYPE_DEPRECATED
};

struct GooseResultData {
	//! The underlying query result
	unique_ptr<QueryResult> result;
	// Results can only use either the new API or the old API, not a mix of the two
	// They start off as "none" and switch to one or the other when an API method is used
	CAPIResultSetType result_set_type;
};

goose_type LogicalTypeIdToC(const LogicalTypeId type);
LogicalTypeId LogicalTypeIdFromC(const goose_type type);
idx_t GetCTypeSize(const goose_type type);
goose_statement_type StatementTypeToC(const StatementType type);
goose_error_type ErrorTypeToC(const ExceptionType type);
ExceptionType ErrorTypeFromC(const goose_error_type type);

goose_state GooseTranslateResult(unique_ptr<QueryResult> result, goose_result *out);
bool DeprecatedMaterializeResult(goose_result *result);

} // namespace goose
