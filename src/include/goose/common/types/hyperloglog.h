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

#include <goose/common/bit_utils.h>
#include <goose/common/types/vector.h>
#include <goose/common/types-import.h>

namespace goose {

enum class HLLStorageType : uint8_t {
	HLL_V1 = 1, //! Redis HLL
	HLL_V2 = 2, //! Our own implementation
};

class Serializer;
class Deserializer;

//! Algorithms from
//! "New cardinality estimation algorithms for HyperLogLog sketches"
//! Otmar Ertl, arXiv:1702.01284
class HyperLogLog {
public:
	static constexpr idx_t P = 6;
	static constexpr idx_t Q = 64 - P;
	static constexpr idx_t M = 1 << P;
	static constexpr double ALPHA = 0.721347520444481703680; // 1 / (2 log(2))

	static double GetErrorRate() {
		return std::sqrt(PI / 2.0) / sqrt(M);
	}

public:
	HyperLogLog() {
		memset(k, 0, sizeof(k));
	}

	//! Algorithm 1
	inline void InsertElement(hash_t h) {
		const auto i = h & ((1 << P) - 1);
		h >>= P;
		h |= hash_t(1) << Q;
		const uint8_t z = UnsafeNumericCast<uint8_t>(CountZeros<hash_t>::Trailing(h) + 1);
		Update(i, z);
	}

	inline void Update(const idx_t &i, const uint8_t &z) {
		k[i] = MaxValue<uint8_t>(k[i], z);
	}

	inline uint8_t GetRegister(const idx_t &i) const {
		return k[i];
	}

	idx_t Count() const;

	//! Algorithm 2
	void Merge(const HyperLogLog &other);

public:
	//! Add data to this HLL
	void Update(Vector &input, Vector &hashes, idx_t count);
	//! Get copy of the HLL
	unique_ptr<HyperLogLog> Copy() const;

	void Serialize(Serializer &serializer) const;
	static unique_ptr<HyperLogLog> Deserialize(Deserializer &deserializer);

	//! Algorithm 4
	void ExtractCounts(uint32_t *c) const;
	//! Algorithm 6
	static int64_t EstimateCardinality(uint32_t *c);

private:
	uint8_t k[M];
};

} // namespace goose
