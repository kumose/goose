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
