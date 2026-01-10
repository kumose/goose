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

#include "parquet_dbp_encoder.h"
#include <goose/common/serializer/memory_stream.h>

namespace goose {

class DlbaEncoder {
public:
	DlbaEncoder(const idx_t total_value_count_p, const idx_t total_string_size_p)
	    : dbp_encoder(total_value_count_p), total_string_size(total_string_size_p) {
	}

public:
	template <class T>
	void BeginWrite(Allocator &, WriteStream &, const T &) {
		throw InternalException("DlbaEncoder should only be used with strings");
	}

	template <class T>
	void WriteValue(WriteStream &, const T &) {
		throw InternalException("DlbaEncoder should only be used with strings");
	}

	void FinishWrite(WriteStream &writer) {
		dbp_encoder.FinishWrite(writer);
		writer.WriteData(buffer.get(), stream->GetPosition());
	}

	template <class SRC>
	static idx_t GetStringSize(const SRC &) {
		return 0;
	}

private:
	DbpEncoder dbp_encoder;
	const idx_t total_string_size;
	AllocatedData buffer;
	unsafe_unique_ptr<MemoryStream> stream;
};

template <>
inline void DlbaEncoder::BeginWrite(Allocator &allocator, WriteStream &writer, const string_t &first_value) {
	buffer = allocator.Allocate(total_string_size + 1);
	stream = make_unsafe_uniq<MemoryStream>(buffer.get(), buffer.GetSize());
	dbp_encoder.BeginWrite(writer, UnsafeNumericCast<int64_t>(first_value.GetSize()));
	stream->WriteData(const_data_ptr_cast(first_value.GetData()), first_value.GetSize());
}

template <>
inline void DlbaEncoder::WriteValue(WriteStream &writer, const string_t &value) {
	dbp_encoder.WriteValue(writer, UnsafeNumericCast<int64_t>(value.GetSize()));
	stream->WriteData(const_data_ptr_cast(value.GetData()), value.GetSize());
}

template <>
inline idx_t DlbaEncoder::GetStringSize(const string_t &src_value) {
	return src_value.GetSize();
}

} // namespace goose
