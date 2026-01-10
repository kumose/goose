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

#include <goose/parser/parsed_data/create_info.h>

namespace goose {

struct CreateSchemaInfo : public CreateInfo {
	CreateSchemaInfo();

public:
	GOOSE_API void Serialize(Serializer &serializer) const override;
	GOOSE_API static unique_ptr<CreateInfo> Deserialize(Deserializer &deserializer);

	unique_ptr<CreateInfo> Copy() const override;
	string ToString() const override;
};

} // namespace goose
