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
