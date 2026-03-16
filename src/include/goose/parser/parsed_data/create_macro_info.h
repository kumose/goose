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
