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

#include <goose/storage/metadata/metadata_manager.h>
#include <goose/common/serializer/write_stream.h>

namespace goose {

class MetadataWriter : public WriteStream {
public:
	explicit MetadataWriter(MetadataManager &manager,
	                        optional_ptr<vector<MetaBlockPointer>> written_pointers = nullptr);
	MetadataWriter(const MetadataWriter &) = delete;
	MetadataWriter &operator=(const MetadataWriter &) = delete;
	~MetadataWriter() override;

public:
	void WriteData(const_data_ptr_t buffer, idx_t write_size) override;
	void Flush();

	BlockPointer GetBlockPointer();
	MetaBlockPointer GetMetaBlockPointer();
	MetadataManager &GetManager() {
		return manager;
	}
	void SetWrittenPointers(optional_ptr<vector<MetaBlockPointer>> written_pointers);

protected:
	virtual MetadataHandle NextHandle();

private:
	data_ptr_t BasePtr();
	data_ptr_t Ptr();

	void NextBlock();

private:
	MetadataManager &manager;
	MetadataHandle block;
	MetadataPointer current_pointer;
	optional_ptr<vector<MetaBlockPointer>> written_pointers;
	idx_t capacity;
	idx_t offset;
};

} // namespace goose
