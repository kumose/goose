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

#include <goose/catalog/standard_entry.h>
#include <goose/parser/statement/select_statement.h>
#include <goose/common/types.h>
#include <goose/common/vector.h>

namespace goose {

class DataTable;
struct CreateViewInfo;

//! A view catalog entry
class ViewCatalogEntry : public StandardEntry {
public:
	static constexpr const CatalogType Type = CatalogType::VIEW_ENTRY;
	static constexpr const char *Name = "view";

public:
	//! Create a real TableCatalogEntry and initialize storage for it
	ViewCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateViewInfo &info);

	//! The query of the view
	unique_ptr<SelectStatement> query;
	//! The SQL query (if any)
	string sql;
	//! The set of aliases associated with the view
	vector<string> aliases;
	//! The returned types of the view
	vector<LogicalType> types;
	//! The returned names of the view
	vector<string> names;
	//! The comments on the columns of the view: can be empty if there are no comments
	vector<Value> column_comments;

public:
	unique_ptr<CreateInfo> GetInfo() const override;

	unique_ptr<CatalogEntry> AlterEntry(ClientContext &context, AlterInfo &info) override;

	unique_ptr<CatalogEntry> Copy(ClientContext &context) const override;

	virtual const SelectStatement &GetQuery();

	virtual bool HasTypes() const {
		// Whether or not the view has types/names defined
		return true;
	}

	string ToSQL() const override;

private:
	void Initialize(CreateViewInfo &info);
};
} // namespace goose
