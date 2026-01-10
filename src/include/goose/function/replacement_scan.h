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

#include <goose/common/common.h>
#include <goose/common/string_util.h>
#include <goose/common/enums/file_compression_type.h>

namespace goose {

class ClientContext;
class TableRef;

struct ReplacementScanData {
public:
	virtual ~ReplacementScanData() {
	}

public:
	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}
};

struct ReplacementScanInput {
public:
	explicit ReplacementScanInput(const string &catalog_name, const string &schema_name, const string &table_name)
	    : catalog_name(catalog_name), schema_name(schema_name), table_name(table_name) {
	}

public:
	const string &catalog_name;
	const string &schema_name;
	const string &table_name;
};

typedef unique_ptr<TableRef> (*replacement_scan_t)(ClientContext &context, ReplacementScanInput &input,
                                                   optional_ptr<ReplacementScanData> data);

//! Replacement table scans are automatically attempted when a table name cannot be found in the schema
//! This allows you to do e.g. SELECT * FROM 'filename.csv', and automatically convert this into a CSV scan
struct ReplacementScan {
	explicit ReplacementScan(replacement_scan_t function, unique_ptr<ReplacementScanData> data_p = nullptr)
	    : function(function), data(std::move(data_p)) {
	}

	static bool CanReplace(const string &table_name, const vector<string> &extensions) {
		auto lower_name = StringUtil::Lower(table_name);

		if (StringUtil::EndsWith(lower_name, CompressionExtensionFromType(FileCompressionType::GZIP))) {
			lower_name = lower_name.substr(0, lower_name.size() - 3);
		} else if (StringUtil::EndsWith(lower_name, CompressionExtensionFromType(FileCompressionType::ZSTD))) {
			lower_name = lower_name.substr(0, lower_name.size() - 4);
		}

		for (auto &extension : extensions) {
			if (StringUtil::EndsWith(lower_name, "." + extension) ||
			    StringUtil::Contains(lower_name, "." + extension + "?")) {
				return true;
			}
		}

		return false;
	}

	static string GetFullPath(const string &catalog, const string &schema, const string &table) {
		string table_name = catalog;
		if (!schema.empty()) {
			table_name += (!table_name.empty() ? "." : "") + schema;
		}
		table_name += (!table_name.empty() ? "." : "") + table;
		return table_name;
	}

	static string GetFullPath(ReplacementScanInput &input) {
		return GetFullPath(input.catalog_name, input.schema_name, input.table_name);
	}

	replacement_scan_t function;
	unique_ptr<ReplacementScanData> data;
};

} // namespace goose
