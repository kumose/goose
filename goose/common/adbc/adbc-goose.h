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

#include "goose/common/adbc/adbc.h"

#include <goose/main/capi/capi_internal.h>

#include <string>

namespace goose_adbc {

class AppenderWrapper {
public:
	AppenderWrapper(goose_connection conn, const char *schema, const char *table) : appender(nullptr) {
		if (goose_appender_create(conn, schema, table, &appender) != GooseSuccess) {
			appender = nullptr;
		}
	}
	~AppenderWrapper() {
		if (appender) {
			goose_appender_destroy(&appender);
		}
	}

	goose_appender Get() const {
		return appender;
	}
	bool Valid() const {
		return appender != nullptr;
	}

private:
	goose_appender appender;
};

class DataChunkWrapper {
public:
	DataChunkWrapper() : chunk(nullptr) {
	}

	~DataChunkWrapper() {
		if (chunk) {
			goose_destroy_data_chunk(&chunk);
		}
	}

	explicit operator goose_data_chunk() const {
		return chunk;
	}

	goose_data_chunk chunk;
};

class ConvertedSchemaWrapper {
public:
	ConvertedSchemaWrapper() : schema(nullptr) {
	}
	~ConvertedSchemaWrapper() {
		if (schema) {
			goose_destroy_arrow_converted_schema(&schema);
		}
	}
	goose_arrow_converted_schema *GetPtr() {
		return &schema;
	}

	explicit operator goose_arrow_converted_schema() const {
		return schema;
	}
	goose_arrow_converted_schema Get() const {
		return schema;
	}

private:
	goose_arrow_converted_schema schema;
};

AdbcStatusCode DatabaseNew(struct AdbcDatabase *database, struct AdbcError *error);

AdbcStatusCode DatabaseSetOption(struct AdbcDatabase *database, const char *key, const char *value,
                                 struct AdbcError *error);

AdbcStatusCode DatabaseInit(struct AdbcDatabase *database, struct AdbcError *error);

AdbcStatusCode DatabaseRelease(struct AdbcDatabase *database, struct AdbcError *error);

AdbcStatusCode ConnectionNew(struct AdbcConnection *connection, struct AdbcError *error);

AdbcStatusCode ConnectionSetOption(struct AdbcConnection *connection, const char *key, const char *value,
                                   struct AdbcError *error);

AdbcStatusCode ConnectionInit(struct AdbcConnection *connection, struct AdbcDatabase *database,
                              struct AdbcError *error);

AdbcStatusCode ConnectionRelease(struct AdbcConnection *connection, struct AdbcError *error);

AdbcStatusCode ConnectionGetInfo(struct AdbcConnection *connection, const uint32_t *info_codes,
                                 size_t info_codes_length, struct ArrowArrayStream *out, struct AdbcError *error);

AdbcStatusCode ConnectionGetObjects(struct AdbcConnection *connection, int depth, const char *catalog,
                                    const char *db_schema, const char *table_name, const char **table_type,
                                    const char *column_name, struct ArrowArrayStream *out, struct AdbcError *error);

AdbcStatusCode ConnectionGetTableSchema(struct AdbcConnection *connection, const char *catalog, const char *db_schema,
                                        const char *table_name, struct ArrowSchema *schema, struct AdbcError *error);

AdbcStatusCode ConnectionGetTableTypes(struct AdbcConnection *connection, struct ArrowArrayStream *out,
                                       struct AdbcError *error);

AdbcStatusCode ConnectionReadPartition(struct AdbcConnection *connection, const uint8_t *serialized_partition,
                                       size_t serialized_length, struct ArrowArrayStream *out, struct AdbcError *error);

AdbcStatusCode ConnectionCommit(struct AdbcConnection *connection, struct AdbcError *error);

AdbcStatusCode ConnectionRollback(struct AdbcConnection *connection, struct AdbcError *error);

AdbcStatusCode StatementNew(struct AdbcConnection *connection, struct AdbcStatement *statement,
                            struct AdbcError *error);

AdbcStatusCode StatementRelease(struct AdbcStatement *statement, struct AdbcError *error);

AdbcStatusCode StatementExecuteQuery(struct AdbcStatement *statement, struct ArrowArrayStream *out,
                                     int64_t *rows_affected, struct AdbcError *error);

AdbcStatusCode StatementPrepare(struct AdbcStatement *statement, struct AdbcError *error);

AdbcStatusCode StatementSetSqlQuery(struct AdbcStatement *statement, const char *query, struct AdbcError *error);

AdbcStatusCode StatementSetSubstraitPlan(struct AdbcStatement *statement, const uint8_t *plan, size_t length,
                                         struct AdbcError *error);

AdbcStatusCode StatementBind(struct AdbcStatement *statement, struct ArrowArray *values, struct ArrowSchema *schema,
                             struct AdbcError *error);

AdbcStatusCode StatementBindStream(struct AdbcStatement *statement, struct ArrowArrayStream *stream,
                                   struct AdbcError *error);

AdbcStatusCode StatementGetParameterSchema(struct AdbcStatement *statement, struct ArrowSchema *schema,
                                           struct AdbcError *error);

AdbcStatusCode StatementSetOption(struct AdbcStatement *statement, const char *key, const char *value,
                                  struct AdbcError *error);

AdbcStatusCode StatementExecutePartitions(struct AdbcStatement *statement, struct ArrowSchema *schema,
                                          struct AdbcPartitions *partitions, int64_t *rows_affected,
                                          struct AdbcError *error);

void InitializeADBCError(AdbcError *error);

} // namespace goose_adbc

//! This method should only be called when the string is guaranteed to not be NULL
void SetError(struct AdbcError *error, const std::string &message);
