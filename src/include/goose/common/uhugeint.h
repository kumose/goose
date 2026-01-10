#pragma once

#include <goose/common/winapi.h>
#include <goose/common/string.h>
#include <stdint.h>

namespace goose {

// Forward declaration to allow conversion between hugeint and uhugeint
struct hugeint_t; // NOLINT

struct uhugeint_t { // NOLINT
public:
	uint64_t lower;
	uint64_t upper;

public:
	uhugeint_t() = default;
	GOOSE_API uhugeint_t(uint64_t value); // NOLINT: Allow implicit conversion from `uint64_t`
	constexpr uhugeint_t(uint64_t upper, uint64_t lower) : lower(lower), upper(upper) {
	}
	constexpr uhugeint_t(const uhugeint_t &rhs) = default;
	constexpr uhugeint_t(uhugeint_t &&rhs) = default;
	uhugeint_t &operator=(const uhugeint_t &rhs) = default;
	uhugeint_t &operator=(uhugeint_t &&rhs) = default;

	GOOSE_API string ToString() const;

	// comparison operators
	GOOSE_API bool operator==(const uhugeint_t &rhs) const;
	GOOSE_API bool operator!=(const uhugeint_t &rhs) const;
	GOOSE_API bool operator<=(const uhugeint_t &rhs) const;
	GOOSE_API bool operator<(const uhugeint_t &rhs) const;
	GOOSE_API bool operator>(const uhugeint_t &rhs) const;
	GOOSE_API bool operator>=(const uhugeint_t &rhs) const;

	// arithmetic operators
	GOOSE_API uhugeint_t operator+(const uhugeint_t &rhs) const;
	GOOSE_API uhugeint_t operator-(const uhugeint_t &rhs) const;
	GOOSE_API uhugeint_t operator*(const uhugeint_t &rhs) const;
	GOOSE_API uhugeint_t operator/(const uhugeint_t &rhs) const;
	GOOSE_API uhugeint_t operator%(const uhugeint_t &rhs) const;
	GOOSE_API uhugeint_t operator-() const;

	// bitwise operators
	GOOSE_API uhugeint_t operator>>(const uhugeint_t &rhs) const;
	GOOSE_API uhugeint_t operator<<(const uhugeint_t &rhs) const;
	GOOSE_API uhugeint_t operator&(const uhugeint_t &rhs) const;
	GOOSE_API uhugeint_t operator|(const uhugeint_t &rhs) const;
	GOOSE_API uhugeint_t operator^(const uhugeint_t &rhs) const;
	GOOSE_API uhugeint_t operator~() const;

	// in-place operators
	GOOSE_API uhugeint_t &operator+=(const uhugeint_t &rhs);
	GOOSE_API uhugeint_t &operator-=(const uhugeint_t &rhs);
	GOOSE_API uhugeint_t &operator*=(const uhugeint_t &rhs);
	GOOSE_API uhugeint_t &operator/=(const uhugeint_t &rhs);
	GOOSE_API uhugeint_t &operator%=(const uhugeint_t &rhs);
	GOOSE_API uhugeint_t &operator>>=(const uhugeint_t &rhs);
	GOOSE_API uhugeint_t &operator<<=(const uhugeint_t &rhs);
	GOOSE_API uhugeint_t &operator&=(const uhugeint_t &rhs);
	GOOSE_API uhugeint_t &operator|=(const uhugeint_t &rhs);
	GOOSE_API uhugeint_t &operator^=(const uhugeint_t &rhs);

	// boolean operators
	GOOSE_API explicit operator bool() const;
	GOOSE_API bool operator!() const;

	// cast operators -- doesn't check bounds/overflow/underflow
	GOOSE_API explicit operator uint8_t() const;
	GOOSE_API explicit operator uint16_t() const;
	GOOSE_API explicit operator uint32_t() const;
	GOOSE_API explicit operator uint64_t() const;
	GOOSE_API explicit operator int8_t() const;
	GOOSE_API explicit operator int16_t() const;
	GOOSE_API explicit operator int32_t() const;
	GOOSE_API explicit operator int64_t() const;
	GOOSE_API operator hugeint_t() const; // NOLINT: Allow implicit conversion from `uhugeint_t`
};

} // namespace goose

namespace std {
template <>
struct hash<goose::uhugeint_t> {
	size_t operator()(const goose::uhugeint_t &val) const {
		using std::hash;
		return hash<uint64_t> {}(val.upper) ^ hash<uint64_t> {}(val.lower);
	}
};
} // namespace std
