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

#include <goose/goose.h>
#include <goose/catalog/catalog.h>
#include <goose/common/types/data_chunk.h>

namespace goose {
class ClientContext;
}

namespace tpcds {

struct DSDGenWrapper {
	//! Create the TPC-DS tables in the given schema with the given suffix
	static void CreateTPCDSSchema(goose::ClientContext &context, std::string catalog, std::string schema, std::string suffix, bool keys,
	                              bool overwrite);
	//! Generate the TPC-DS data of the given scale factor
	static void DSDGen(double scale, goose::ClientContext &context, std::string catalog, std::string schema, std::string suffix);

	static uint32_t QueriesCount();
	//! Gets the specified TPC-DS Query number as a string
	static std::string GetQuery(int query);
	//! Returns the CSV answer of a TPC-DS query
	static std::string GetAnswer(double sf, int query);
};

} // namespace tpcds
