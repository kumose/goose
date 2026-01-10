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

#include <goose/parser/statement/select_statement.h>
#include <goose/parser/tableref.h>

namespace goose {
//! Represents a subquery
class SubqueryRef : public TableRef {
public:
	static constexpr const TableReferenceType TYPE = TableReferenceType::SUBQUERY;

private:
	SubqueryRef();

public:
	GOOSE_API explicit SubqueryRef(unique_ptr<SelectStatement> subquery, string alias = string());

	//! The subquery
	unique_ptr<SelectStatement> subquery;

public:
	string ToString() const override;
	bool Equals(const TableRef &other_p) const override;

	unique_ptr<TableRef> Copy() override;

	//! Deserializes a blob back into a SubqueryRef
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<TableRef> Deserialize(Deserializer &source);
};
} // namespace goose
