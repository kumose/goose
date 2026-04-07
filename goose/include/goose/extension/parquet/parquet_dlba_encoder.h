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

#include <goose/extension/parquet/parquet_dbp_encoder.h>
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
