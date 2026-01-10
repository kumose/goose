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
