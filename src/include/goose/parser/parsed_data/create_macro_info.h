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

#include <goose/parser/parsed_data/create_function_info.h>
#include <goose/function/macro_function.h>

namespace goose {

struct CreateMacroInfo : public CreateFunctionInfo {
	explicit CreateMacroInfo(CatalogType type);

	vector<unique_ptr<MacroFunction>> macros;

public:
	unique_ptr<CreateInfo> Copy() const override;

	string ToString() const override;
	GOOSE_API void Serialize(Serializer &serializer) const override;
	GOOSE_API static unique_ptr<CreateInfo> Deserialize(Deserializer &deserializer);

	//! This is a weird function that exists only for backwards compatibility of serialization
	//! Essentially we used to only support a single function in the CreateMacroInfo
	//! In order to not break backwards/forwards compatibility, we serialize the first function in the old manner
	//! Extra functions are serialized if present in a separate field
	vector<unique_ptr<MacroFunction>> GetAllButFirstFunction() const;
	//! This is a weird constructor that exists only for serialization, similarly to GetAllButFirstFunction
	CreateMacroInfo(CatalogType type, unique_ptr<MacroFunction> function,
	                vector<unique_ptr<MacroFunction>> extra_functions);
};

} // namespace goose
