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
#include <goose/common/enums/catalog_type.h>
#include <goose/parser/parsed_data/parse_info.h>

namespace goose {

enum class ExtraDropInfoType : uint8_t {
	INVALID = 0,

	SECRET_INFO = 1
};

struct ExtraDropInfo {
	explicit ExtraDropInfo(ExtraDropInfoType info_type) : info_type(info_type) {
	}
	virtual ~ExtraDropInfo() {
	}

	ExtraDropInfoType info_type;

public:
	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}

	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}
	virtual unique_ptr<ExtraDropInfo> Copy() const = 0;

	virtual void Serialize(Serializer &serializer) const;
	static unique_ptr<ExtraDropInfo> Deserialize(Deserializer &deserializer);
};

struct ExtraDropSecretInfo : public ExtraDropInfo {
	ExtraDropSecretInfo();
	ExtraDropSecretInfo(const ExtraDropSecretInfo &info);

	//! Secret Persistence
	SecretPersistType persist_mode;
	//! (optional) the name of the storage to drop from
	string secret_storage;

public:
	unique_ptr<ExtraDropInfo> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ExtraDropInfo> Deserialize(Deserializer &deserializer);
};

} // namespace goose
