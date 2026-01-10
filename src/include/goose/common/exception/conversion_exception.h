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

#include <goose/common/exception.h>
#include <goose/common/optional_idx.h>

namespace goose {
class ConversionException : public Exception {
public:
	GOOSE_API explicit ConversionException(const string &msg);

	GOOSE_API explicit ConversionException(optional_idx error_location, const string &msg);

	GOOSE_API ConversionException(const PhysicalType orig_type, const PhysicalType new_type);

	GOOSE_API ConversionException(const LogicalType &orig_type, const LogicalType &new_type);

	template <typename... ARGS>
	explicit ConversionException(const string &msg, ARGS &&...params)
	    : ConversionException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}

	template <typename... ARGS>
	explicit ConversionException(optional_idx error_location, const string &msg, ARGS &&...params)
	    : ConversionException(error_location, ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};
} // namespace goose
