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

#include <goose/catalog/standard_entry.h>
#include <goose/common/types-import.h>
#include <goose/parser/parsed_data/create_sequence_info.h>
#include <goose/parser/parsed_data/alter_table_info.h>

namespace goose {
class GooseTransaction;
class SequenceCatalogEntry;

struct SequenceValue {
	SequenceCatalogEntry *entry;
	uint64_t usage_count;
	int64_t counter;
};

struct SequenceData {
	explicit SequenceData(CreateSequenceInfo &info);

	//! The amount of times the sequence has been used
	uint64_t usage_count;
	//! The sequence counter
	int64_t counter;
	//! The most recently returned value
	int64_t last_value;
	//! The increment value
	int64_t increment;
	//! The minimum value of the sequence
	int64_t start_value;
	//! The minimum value of the sequence
	int64_t min_value;
	//! The maximum value of the sequence
	int64_t max_value;
	//! Whether or not the sequence cycles
	bool cycle;
};

//! A sequence catalog entry
class SequenceCatalogEntry : public StandardEntry {
public:
	static constexpr const CatalogType Type = CatalogType::SEQUENCE_ENTRY;
	static constexpr const char *Name = "sequence";

public:
	//! Create a real TableCatalogEntry and initialize storage for it
	SequenceCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateSequenceInfo &info);

public:
	unique_ptr<CatalogEntry> Copy(ClientContext &context) const override;
	unique_ptr<CreateInfo> GetInfo() const override;

	SequenceData GetData() const;
	int64_t CurrentValue();
	int64_t NextValue(GooseTransaction &transaction);
	void ReplayValue(uint64_t usage_count, int64_t counter);

	string ToSQL() const override;

private:
	//! Lock for getting a value on the sequence
	mutable mutex lock;
	//! Sequence data
	SequenceData data;
};
} // namespace goose
