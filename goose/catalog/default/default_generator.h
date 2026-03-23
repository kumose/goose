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
