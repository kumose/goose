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

#include <goose/common/types/value.h>
#include <goose/common/optional_idx.h>

namespace goose {

// Invalid is 255 because previously stored databases have SampleMethods according to the current ENUMS and we need to
// maintain that
enum class SampleMethod : uint8_t { SYSTEM_SAMPLE = 0, BERNOULLI_SAMPLE = 1, RESERVOIR_SAMPLE = 2, INVALID = 255 };

// **DEPRECATED**: Use EnumUtil directly instead.
string SampleMethodToString(SampleMethod method);

class SampleOptions {
public:
	// 1 billion rows should be enough.
	static constexpr idx_t MAX_SAMPLE_ROWS = 1000000000;

public:
	explicit SampleOptions(int64_t seed_ = -1);

	Value sample_size;
	bool is_percentage;
	SampleMethod method;
	optional_idx seed = optional_idx::Invalid();
	bool repeatable;

	unique_ptr<SampleOptions> Copy();
	void SetSeed(idx_t new_seed);
	static bool Equals(SampleOptions *a, SampleOptions *b);
	void Serialize(Serializer &serializer) const;
	static unique_ptr<SampleOptions> Deserialize(Deserializer &deserializer);
	int64_t GetSeed() const;
};

} // namespace goose
