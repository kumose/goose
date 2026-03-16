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

#include <goose/catalog/catalog_entry/sequence_catalog_entry.h>
#include <goose/function/scalar_function.h>
#include <goose/function/function_set.h>
#include <goose/function/built_in_functions.h>

namespace goose {

struct NextvalBindData : public FunctionData {
	explicit NextvalBindData(SequenceCatalogEntry &sequence) : sequence(sequence), create_info(sequence.GetInfo()) {
	}

	//! The sequence to use for the nextval computation; only if the sequence is a constant
	SequenceCatalogEntry &sequence;

	//! The CreateInfo for the above sequence, if it exists
	unique_ptr<CreateInfo> create_info;

	unique_ptr<FunctionData> Copy() const override {
		return make_uniq<NextvalBindData>(sequence);
	}

	bool Equals(const FunctionData &other_p) const override {
		auto &other = other_p.Cast<NextvalBindData>();
		return RefersToSameObject(sequence, other.sequence);
	}
};

} // namespace goose
