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

#include <goose/main/secret/secret.h>
#include <goose/parser/parsed_data/parse_info.h>
#include <goose/parser/parsed_data/create_info.h>
#include <goose/common/named_parameter_map.h>

namespace goose {

struct CreateSecretInfo : public CreateInfo { // NOLINT: work-around bug in clang-tidy
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::CREATE_SECRET_INFO;

public:
	explicit CreateSecretInfo(OnCreateConflict on_conflict, SecretPersistType persist_type);
	~CreateSecretInfo() override;

	//! How to handle conflict
	OnCreateConflict on_conflict;
	//! Whether the secret can be persisted
	SecretPersistType persist_type;
	//! The type of secret
	unique_ptr<ParsedExpression> type;
	//! Which storage to use (empty for default)
	string storage_type;
	//! (optionally) the provider of the secret credentials
	unique_ptr<ParsedExpression> provider;
	//! (optionally) the name of the secret
	string name;
	//! (optionally) the scope of the secret
	unique_ptr<ParsedExpression> scope;
	//! Named parameter list (if any)
	case_insensitive_map_t<unique_ptr<ParsedExpression>> options;

	unique_ptr<CreateInfo> Copy() const override;
};
} // namespace goose
