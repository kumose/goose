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
