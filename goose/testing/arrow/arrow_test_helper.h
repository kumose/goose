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

#include <utility>

#include <goose/testing/test_helpers.h>
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
