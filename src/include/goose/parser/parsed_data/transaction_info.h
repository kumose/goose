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

namespace goose {

enum class TransactionType : uint8_t { INVALID, BEGIN_TRANSACTION, COMMIT, ROLLBACK };

enum class TransactionModifierType : uint8_t {
	TRANSACTION_DEFAULT_MODIFIER,
	TRANSACTION_READ_ONLY,
	TRANSACTION_READ_WRITE
};

struct TransactionInfo : public ParseInfo {
public:
	static constexpr const ParseInfoType TYPE = ParseInfoType::TRANSACTION_INFO;

public:
	explicit TransactionInfo(TransactionType type);

	//! The type of transaction statement
	TransactionType type;
	//! Whether or not a transaction can make modifications to the database
	TransactionModifierType modifier;

public:
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParseInfo> Deserialize(Deserializer &deserializer);

	string ToString() const;
	unique_ptr<TransactionInfo> Copy() const;

private:
	TransactionInfo();
};

} // namespace goose
