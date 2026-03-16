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
