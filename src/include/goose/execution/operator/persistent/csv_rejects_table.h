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
