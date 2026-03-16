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

#include <goose/storage/object_cache.h>
#include <goose/common/types-import.h>
#include <goose/common/typedefs.h>
#include <goose/common/shared_ptr.h>
#include <goose/common/string.h>

namespace goose {

struct ReadCSVData;
class TableCatalogEntry;
class ClientContext;

class CSVRejectsTable : public ObjectCacheEntry {
public:
	CSVRejectsTable(string rejects_scan, string rejects_error)
	    : count(0), scan_table(std::move(rejects_scan)), errors_table(std::move(rejects_error)) {
	}
	mutex write_lock;
	string name;
	idx_t count;
	string scan_table;
	string errors_table;

	static shared_ptr<CSVRejectsTable> GetOrCreate(ClientContext &context, const string &rejects_scan,
	                                               const string &rejects_error);

	void InitializeTable(ClientContext &context, const ReadCSVData &options);
	TableCatalogEntry &GetErrorsTable(ClientContext &context);
	TableCatalogEntry &GetScansTable(ClientContext &context);

	idx_t GetCurrentFileIndex(idx_t query_id);

	static string ObjectType() {
		return "csv_rejects_table_cache";
	}

	string GetObjectType() override {
		return ObjectType();
	}

	// Rejects table records the overall error counts, which is relatively small and should not be evicted.
	optional_idx GetEstimatedCacheMemory() const override {
		return optional_idx {};
	}

private:
	//! Current File Index being used in the query
	idx_t current_file_idx = 0;
	//! Current Query ID being executed
	idx_t current_query_id = 0;
};

} // namespace goose
