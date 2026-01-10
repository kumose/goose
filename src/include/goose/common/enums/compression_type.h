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
