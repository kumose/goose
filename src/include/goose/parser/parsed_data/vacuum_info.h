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

#include <goose/parser/parsed_data/parse_info.h>
#include <goose/parser/tableref.h>

namespace goose {
class Serializer;
class Deserializer;

struct VacuumOptions {
	VacuumOptions() : vacuum(false), analyze(false) {
	}

	bool vacuum;
	bool analyze;

	void Serialize(Serializer &serializer) const;
	static VacuumOptions Deserialize(Deserializer &deserializer);
};

struct VacuumInfo : public ParseInfo {
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::VACUUM_INFO;

public:
	explicit VacuumInfo(VacuumOptions options);

	const VacuumOptions options;
	vector<string> columns;
	bool has_table;
	unique_ptr<TableRef> ref;

public:
	unique_ptr<VacuumInfo> Copy();
	string ToString() const;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParseInfo> Deserialize(Deserializer &deserializer);
};

} // namespace goose
