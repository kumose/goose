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

#include <goose/common/constants.h>
#include <goose/common/vector.h>
#include <goose/common/optional_ptr.h>

namespace goose {

class StorageManager;

enum class CompressionType : uint8_t {
	COMPRESSION_AUTO = 0,
	COMPRESSION_UNCOMPRESSED = 1,
	COMPRESSION_CONSTANT = 2, // internal only
	COMPRESSION_RLE = 3,
	COMPRESSION_DICTIONARY = 4,
	COMPRESSION_PFOR_DELTA = 5,
	COMPRESSION_BITPACKING = 6,
	COMPRESSION_FSST = 7,
	COMPRESSION_CHIMP = 8,
	COMPRESSION_PATAS = 9,
	COMPRESSION_ALP = 10,
	COMPRESSION_ALPRD = 11,
	COMPRESSION_ZSTD = 12,
	COMPRESSION_ROARING = 13,
	COMPRESSION_EMPTY = 14, // internal only
	COMPRESSION_DICT_FSST = 15,
	COMPRESSION_COUNT // This has to stay the last entry of the type!
};

struct CompressionAvailabilityResult {
private:
	enum class UnavailableReason : uint8_t {
		AVAILABLE,
		//! Introduced later, not available to this version
		NOT_AVAILABLE_YET,
		//! Used to be available, but isnt anymore
		DEPRECATED
	};

public:
	CompressionAvailabilityResult() = default;
	static CompressionAvailabilityResult Deprecated() {
		return CompressionAvailabilityResult(UnavailableReason::DEPRECATED);
	}
	static CompressionAvailabilityResult NotAvailableYet() {
		return CompressionAvailabilityResult(UnavailableReason::NOT_AVAILABLE_YET);
	}

public:
	bool IsAvailable() const {
		return reason == UnavailableReason::AVAILABLE;
	}
	bool IsDeprecated() {
		D_ASSERT(!IsAvailable());
		return reason == UnavailableReason::DEPRECATED;
	}
	bool IsNotAvailableYet() {
		D_ASSERT(!IsAvailable());
		return reason == UnavailableReason::NOT_AVAILABLE_YET;
	}

private:
	explicit CompressionAvailabilityResult(UnavailableReason reason) : reason(reason) {
	}

public:
	UnavailableReason reason = UnavailableReason::AVAILABLE;
};

CompressionAvailabilityResult CompressionTypeIsAvailable(CompressionType compression_type,
                                                         optional_ptr<StorageManager> storage_manager = nullptr);
vector<string> ListCompressionTypes(void);
CompressionType CompressionTypeFromString(const string &str);
string CompressionTypeToString(CompressionType type);

} // namespace goose
