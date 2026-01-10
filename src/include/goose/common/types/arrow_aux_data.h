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

#include <goose/common/types/vector_buffer.h>
#include <goose/common/arrow/arrow_wrapper.h>

namespace goose {

struct ArrowAuxiliaryData : public VectorAuxiliaryData {
	static constexpr const VectorAuxiliaryDataType TYPE = VectorAuxiliaryDataType::ARROW_AUXILIARY;
	explicit ArrowAuxiliaryData(shared_ptr<ArrowArrayWrapper> arrow_array_p)
	    : VectorAuxiliaryData(VectorAuxiliaryDataType::ARROW_AUXILIARY), arrow_array(std::move(arrow_array_p)) {
	}
	~ArrowAuxiliaryData() override {
	}

	shared_ptr<ArrowArrayWrapper> arrow_array;
};

} // namespace goose
