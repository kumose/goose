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

#include <goose/common/arrow/appender/append_data.h>
#include <goose/function/table/arrow.h>

#include <goose/common/bswap.h>

namespace goose {

//===--------------------------------------------------------------------===//
// Scalar Types
//===--------------------------------------------------------------------===//
struct ArrowScalarConverter {
	template <class TGT, class SRC>
	static TGT Operation(SRC input) {
		return input;
	}

	static bool SkipNulls() {
		return false;
	}

	template <class TGT>
	static void SetNull(TGT &value) {
	}
};

struct ArrowIntervalConverter {
	template <class TGT, class SRC>
	static TGT Operation(SRC input) {
		ArrowInterval result;
		result.months = input.months;
		result.days = input.days;
		result.nanoseconds = input.micros * Interval::NANOS_PER_MICRO;
		return result;
	}

	static bool SkipNulls() {
		return true;
	}

	template <class TGT>
	static void SetNull(TGT &value) {
	}
};

struct ArrowTimeTzConverter {
	template <class TGT, class SRC>
	static TGT Operation(SRC input) {
		return input.time().micros;
	}

	static bool SkipNulls() {
		return true;
	}

	template <class TGT>
	static void SetNull(TGT &value) {
	}
};

struct ArrowUUIDBlobConverter {
	template <class TGT, class SRC>
	static TGT Operation(hugeint_t input) {
		// Turn into big-end
		auto upper = BSwapIfLE(input.lower);
		// flip Upper MSD
		auto lower =
		    BSwapIfLE(static_cast<int64_t>(static_cast<uint64_t>(input.upper) ^ (static_cast<uint64_t>(1) << 63)));
		return {static_cast<int64_t>(upper), static_cast<uint64_t>(lower)};
	}

	static bool SkipNulls() {
		return true;
	}

	template <class TGT>
	static void SetNull(TGT &value) {
	}
};

template <class TGT, class SRC = TGT, class OP = ArrowScalarConverter>
struct ArrowScalarBaseData {
	static void Append(ArrowAppendData &append_data, Vector &input, idx_t from, idx_t to, idx_t input_size) {
		D_ASSERT(to >= from);
		idx_t size = to - from;
		D_ASSERT(size <= input_size);
		UnifiedVectorFormat format;
		input.ToUnifiedFormat(input_size, format);

		// append the validity mask
		append_data.AppendValidity(format, from, to);

		// append the main data
		auto &main_buffer = append_data.GetMainBuffer();
		main_buffer.resize(main_buffer.size() + sizeof(TGT) * size);
		auto data = UnifiedVectorFormat::GetData<SRC>(format);
		auto result_data = main_buffer.GetData<TGT>();

		for (idx_t i = from; i < to; i++) {
			auto source_idx = format.sel->get_index(i);
			auto result_idx = append_data.row_count + i - from;

			if (OP::SkipNulls() && !format.validity.RowIsValid(source_idx)) {
				OP::template SetNull<TGT>(result_data[result_idx]);
				continue;
			}
			result_data[result_idx] = OP::template Operation<TGT, SRC>(data[source_idx]);
		}
		append_data.row_count += size;
	}
};

template <class TGT, class SRC = TGT, class OP = ArrowScalarConverter>
struct ArrowScalarData : public ArrowScalarBaseData<TGT, SRC, OP> {
	static void Initialize(ArrowAppendData &result, const LogicalType &type, idx_t capacity) {
		result.GetMainBuffer().reserve(capacity * sizeof(TGT));
	}

	static void Finalize(ArrowAppendData &append_data, const LogicalType &type, ArrowArray *result) {
		result->n_buffers = 2;
		result->buffers[1] = append_data.GetMainBuffer().data();
	}
};

} // namespace goose
