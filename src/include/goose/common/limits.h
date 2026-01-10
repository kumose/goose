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

#include <goose/common/hugeint.h>
#include <goose/common/uhugeint.h>
#include <goose/common/types.h>

#include <type_traits>

// Undef annoying windows macro
#undef max

#include <limits>

namespace goose {

template <class T>
struct NumericLimits {
	static constexpr T Minimum() {
		return std::numeric_limits<T>::has_infinity ? -std::numeric_limits<T>::infinity()
		                                            : std::numeric_limits<T>::lowest();
	}
	static constexpr T Maximum() {
		return std::numeric_limits<T>::has_infinity ? std::numeric_limits<T>::infinity()
		                                            : std::numeric_limits<T>::max();
	}
	static constexpr bool IsSigned() {
		return std::is_signed<T>::value;
	}
	static constexpr bool IsIntegral() {
		return std::is_integral<T>::value || std::is_enum<T>::value;
	}
	static constexpr idx_t Digits();
};

template <>
struct NumericLimits<hugeint_t> {
	static constexpr hugeint_t Minimum() {
		return {std::numeric_limits<int64_t>::lowest(), 0};
	};
	static constexpr hugeint_t Maximum() {
		return {std::numeric_limits<int64_t>::max(), std::numeric_limits<uint64_t>::max()};
	};
	static constexpr bool IsSigned() {
		return true;
	}
	static constexpr bool IsIntegral() {
		return true;
	}
	static constexpr idx_t Digits() {
		return 39;
	}
};

template <>
struct NumericLimits<uhugeint_t> {
	static constexpr uhugeint_t Minimum() {
		return {0, 0};
	};
	static constexpr uhugeint_t Maximum() {
		return {std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::max()};
	};
	static constexpr bool IsSigned() {
		return false;
	}
	static constexpr bool IsIntegral() {
		return true;
	}
	static constexpr idx_t Digits() {
		return 39;
	}
};

template <>
constexpr idx_t NumericLimits<int8_t>::Digits() {
	return 3;
}

template <>
constexpr idx_t NumericLimits<int16_t>::Digits() {
	return 5;
}

template <>
constexpr idx_t NumericLimits<int32_t>::Digits() {
	return 10;
}

template <>
constexpr idx_t NumericLimits<int64_t>::Digits() {
	return 19;
}

template <>
constexpr idx_t NumericLimits<uint8_t>::Digits() {
	return 3;
}

template <>
constexpr idx_t NumericLimits<uint16_t>::Digits() {
	return 5;
}

template <>
constexpr idx_t NumericLimits<uint32_t>::Digits() {
	return 10;
}

template <>
constexpr idx_t NumericLimits<uint64_t>::Digits() {
	return 20;
}

template <>
constexpr idx_t NumericLimits<float>::Digits() {
	return 127;
}

template <>
constexpr idx_t NumericLimits<double>::Digits() {
	return 250;
}

} // namespace goose
