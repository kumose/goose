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

#include <goose/common/string.h>
#include <goose/planner/binding_alias.h>
#include <goose/parser/keyword_helper.h>

namespace goose {

struct QualifiedName {
	string catalog;
	string schema;
	string name;

	//! Parse the (optional) schema and a name from a string in the format of e.g. "schema"."table"; if there is no dot
	//! the schema will be set to INVALID_SCHEMA
	static QualifiedName Parse(const string &input);
	static vector<string> ParseComponents(const string &input);
	string ToString() const;
};

struct QualifiedColumnName {
	QualifiedColumnName();
	QualifiedColumnName(string column_p); // NOLINT: allow implicit conversion from string to column name
	QualifiedColumnName(string table_p, string column_p);
	QualifiedColumnName(const BindingAlias &alias, string column_p);

	string catalog;
	string schema;
	string table;
	string column;

	static QualifiedColumnName Parse(string &input);

	string ToString() const;

	void Serialize(Serializer &serializer) const;
	static QualifiedColumnName Deserialize(Deserializer &deserializer);

	bool IsQualified() const;

	bool operator==(const QualifiedColumnName &rhs) const;
};

} // namespace goose
