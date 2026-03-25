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

#include <goose/common/winapi.h>
#include <goose/common/arrow/arrow_wrapper.h>
#include <goose/main/query_result.h>
#include <goose/common/error_data.h>
#include <goose/common/unique_ptr.h>

namespace goose {

class ClientContext;

class ArrowQueryResult : public QueryResult {
public:
	static constexpr QueryResultType TYPE = QueryResultType::ARROW_RESULT;

public:
	friend class ClientContext;
	//! Creates a successful query result with the specified names and types
	GOOSE_API ArrowQueryResult(StatementType statement_type, StatementProperties properties, vector<string> names_p,
	                            vector<LogicalType> types_p, ClientProperties client_properties, idx_t batch_size);
	//! Creates an unsuccessful query result with error condition
	GOOSE_API explicit ArrowQueryResult(ErrorData error);

public:
	//! Converts the QueryResult to a string
	GOOSE_API string ToString() override;

public:
	vector<unique_ptr<ArrowArrayWrapper>> ConsumeArrays();
	vector<unique_ptr<ArrowArrayWrapper>> &Arrays();
	void SetArrowData(vector<unique_ptr<ArrowArrayWrapper>> arrays);
	idx_t BatchSize() const;

protected:
	GOOSE_API unique_ptr<DataChunk> FetchInternal() override;

private:
	vector<unique_ptr<ArrowArrayWrapper>> arrays;
	idx_t batch_size;
};

} // namespace goose
