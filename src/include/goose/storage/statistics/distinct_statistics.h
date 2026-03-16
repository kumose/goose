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

#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/types/hyperloglog.h>

namespace goose {
class Vector;
class Serializer;
class Deserializer;

class DistinctStatistics {
public:
	DistinctStatistics();
	explicit DistinctStatistics(unique_ptr<HyperLogLog> log, idx_t sample_count, idx_t total_count);

	//! The HLL of the table
	unique_ptr<HyperLogLog> log;
	//! How many values have been sampled into the HLL
	atomic<idx_t> sample_count;
	//! How many values have been inserted (before sampling)
	atomic<idx_t> total_count;

public:
	void Merge(const DistinctStatistics &other);

	unique_ptr<DistinctStatistics> Copy() const;

	void UpdateSample(Vector &new_data, idx_t count, Vector &hashes);
	void Update(Vector &new_data, idx_t count, Vector &hashes);

	string ToString() const;
	idx_t GetCount() const;

	static bool TypeIsSupported(const LogicalType &type);

	void Serialize(Serializer &serializer) const;
	static unique_ptr<DistinctStatistics> Deserialize(Deserializer &deserializer);

private:
	void UpdateInternal(Vector &update, idx_t count, Vector &hashes);

private:
	//! For distinct statistics we sample the input to speed up insertions
	static constexpr double BASE_SAMPLE_RATE = 0.1;
	//! For integers, we sample more: likely to be join keys (and hashing is cheaper than, e.g., strings)
	static constexpr double INTEGRAL_SAMPLE_RATE = 0.3;
};

} // namespace goose
