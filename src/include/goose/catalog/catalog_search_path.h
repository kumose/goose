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

#include <functional>
#include <goose/common/enums/catalog_type.h>
#include <goose/common/string.h>
#include <goose/common/vector.h>
#include <goose/common/types/value.h>

namespace goose {

class ClientContext;

struct CatalogSearchEntry {
	CatalogSearchEntry(string catalog, string schema);

	string catalog;
	string schema;

public:
	string ToString() const;
	static string ListToString(const vector<CatalogSearchEntry> &input);
	static CatalogSearchEntry Parse(const string &input);
	static vector<CatalogSearchEntry> ParseList(const string &input);

private:
	static CatalogSearchEntry ParseInternal(const string &input, idx_t &pos);
	static string WriteOptionallyQuoted(const string &input);
};

enum class CatalogSetPathType { SET_SCHEMA, SET_SCHEMAS, SET_DIRECTLY };

//! The schema search path, in order by which entries are searched if no schema entry is provided
class CatalogSearchPath {
public:
	GOOSE_API explicit CatalogSearchPath(ClientContext &client_p);
	GOOSE_API CatalogSearchPath(ClientContext &client_p, vector<CatalogSearchEntry> entries);
	CatalogSearchPath(const CatalogSearchPath &other) = delete;

	GOOSE_API void Set(CatalogSearchEntry new_value, CatalogSetPathType set_type);
	GOOSE_API void Set(vector<CatalogSearchEntry> new_paths, CatalogSetPathType set_type);
	GOOSE_API void Reset();

	GOOSE_API const vector<CatalogSearchEntry> &Get() const;
	const vector<CatalogSearchEntry> &GetSetPaths() const {
		return set_paths;
	}
	GOOSE_API const CatalogSearchEntry &GetDefault() const;
	//! FIXME: this method is deprecated
	GOOSE_API string GetDefaultSchema(const string &catalog) const;
	GOOSE_API string GetDefaultSchema(ClientContext &context, const string &catalog) const;
	GOOSE_API string GetDefaultCatalog(const string &schema) const;

	GOOSE_API vector<string> GetSchemasForCatalog(const string &catalog) const;
	GOOSE_API vector<string> GetCatalogsForSchema(const string &schema) const;

	GOOSE_API bool SchemaInSearchPath(ClientContext &context, const string &catalog_name,
	                                   const string &schema_name) const;

private:
	//! Set paths without checking if they exist
	void SetPathsInternal(vector<CatalogSearchEntry> new_paths);
	string GetSetName(CatalogSetPathType set_type);

private:
	ClientContext &context;
	vector<CatalogSearchEntry> paths;
	//! Only the paths that were explicitly set (minus the always included paths)
	vector<CatalogSearchEntry> set_paths;
};

} // namespace goose
