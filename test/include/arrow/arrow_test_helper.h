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

#include <utility>

#include "test_helpers.h"
#include <goose/common/helper.h>
#include <goose/common/types/value.h>
#include <goose/common/vector.h>
#include <goose/main/connection.h>
#include <goose/main/client_config.h>
#include <goose/main/database.h>
#include <goose/function/table/arrow.h>
#include <goose/common/arrow/arrow_appender.h>
#include <goose/common/arrow/arrow_converter.h>
#include <goose/common/arrow/arrow_wrapper.h>
#include <goose/main/extension_helper.h>
#include <goose/common/arrow/arrow_query_result.h>

class ArrowStreamTestFactory {
public:
	static goose::unique_ptr<goose::ArrowArrayStreamWrapper> CreateStream(uintptr_t this_ptr,
	                                                                        goose::ArrowStreamParameters &parameters);

	static void GetSchema(ArrowArrayStream *arrow_array_stream, ArrowSchema &schema);
};

namespace goose {
class ArrowTestFactory {
public:
	ArrowTestFactory(vector<LogicalType> types_p, vector<string> names_p, goose::unique_ptr<QueryResult> result_p,
	                 bool big_result, ClientProperties options, ClientContext &context)
	    : types(std::move(types_p)), names(std::move(names_p)), result(std::move(result_p)), big_result(big_result),
	      options(std::move(options)), context(context) {
		if (result->type == QueryResultType::ARROW_RESULT) {
			auto &arrow_result = result->Cast<ArrowQueryResult>();
			prefetched_chunks = arrow_result.ConsumeArrays();
			chunk_iterator = prefetched_chunks.begin();
		}
	}

	vector<LogicalType> types;
	vector<string> names;
	goose::unique_ptr<QueryResult> result;
	vector<unique_ptr<ArrowArrayWrapper>> prefetched_chunks;
	vector<unique_ptr<ArrowArrayWrapper>>::iterator chunk_iterator;
	bool big_result;
	ClientProperties options;
	ClientContext &context;

	struct ArrowArrayStreamData {
		explicit ArrowArrayStreamData(ArrowTestFactory &factory, ClientProperties options)
		    : factory(factory), options(options) {
		}

		ArrowTestFactory &factory;
		ClientProperties options;
	};

	static int ArrowArrayStreamGetSchema(struct ArrowArrayStream *stream, struct ArrowSchema *out);

	static int ArrowArrayStreamGetNext(struct ArrowArrayStream *stream, struct ArrowArray *out);

	static const char *ArrowArrayStreamGetLastError(struct ArrowArrayStream *stream);

	static void ArrowArrayStreamRelease(struct ArrowArrayStream *stream);

	static goose::unique_ptr<goose::ArrowArrayStreamWrapper> CreateStream(uintptr_t this_ptr,
	                                                                        ArrowStreamParameters &parameters);

	static void GetSchema(ArrowArrayStream *, ArrowSchema &schema);

	void ToArrowSchema(struct ArrowSchema *out);
};

class ArrowTestHelper {
public:
	//! Used in the Arrow Roundtrip Tests
	static bool RunArrowComparison(Connection &con, const string &query, bool big_result = false);
	//! Used in the ADBC Testing
	static bool RunArrowComparison(Connection &con, const string &query, ArrowArrayStream &arrow_stream);

private:
	static bool CompareResults(Connection &con, unique_ptr<QueryResult> arrow, unique_ptr<MaterializedQueryResult> duck,
	                           const string &query);

public:
	static unique_ptr<QueryResult> ScanArrowObject(Connection &con, vector<Value> &params);
	static vector<Value> ConstructArrowScan(ArrowTestFactory &factory);
	static vector<Value> ConstructArrowScan(ArrowArrayStream &stream);
};
} // namespace goose
