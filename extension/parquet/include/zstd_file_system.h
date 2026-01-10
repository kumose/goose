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

#include <goose/goose.h>
#include <goose/common/compressed_file_system.h>

namespace goose {

class ZStdFileSystem : public CompressedFileSystem {
public:
	unique_ptr<FileHandle> OpenCompressedFile(QueryContext context, unique_ptr<FileHandle> handle, bool write) override;

	std::string GetName() const override {
		return "ZStdFileSystem";
	}

	unique_ptr<StreamWrapper> CreateStream() override;
	idx_t InBufferSize() override;
	idx_t OutBufferSize() override;

	static int64_t DefaultCompressionLevel();
	static int64_t MinimumCompressionLevel();
	static int64_t MaximumCompressionLevel();
};

} // namespace goose
