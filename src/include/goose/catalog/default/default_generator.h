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

#include <goose/catalog/catalog_entry.h>
#include <goose/common/types-import.h>

namespace goose {
class ClientContext;

class DefaultGenerator {
public:
	explicit DefaultGenerator(Catalog &catalog);
	virtual ~DefaultGenerator();

	Catalog &catalog;
	atomic<bool> created_all_entries;

public:
	//! Creates a default entry with the specified name, or returns nullptr if no such entry can be generated
	virtual unique_ptr<CatalogEntry> CreateDefaultEntry(ClientContext &context, const string &entry_name);
	virtual unique_ptr<CatalogEntry> CreateDefaultEntry(CatalogTransaction transaction, const string &entry_name);
	//! Get a list of all default entries in the generator
	virtual vector<string> GetDefaultEntries() = 0;
};

} // namespace goose
