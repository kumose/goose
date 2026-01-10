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
