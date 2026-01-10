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

#include <goose/storage/compression/chimp/chimp.h>
#include <goose/function/compression_function.h>

namespace goose {

struct EmptyChimpWriter;

template <class T>
struct ChimpAnalyzeState : public AnalyzeState {};

template <class T>
unique_ptr<AnalyzeState> ChimpInitAnalyze(ColumnData &col_data, PhysicalType type) {
	// This compression type is deprecated
	return nullptr;
}

template <class T>
bool ChimpAnalyze(AnalyzeState &state, Vector &input, idx_t count) {
	throw InternalException("Chimp has been deprecated, can no longer be used to compress data");
	return false;
}

template <class T>
idx_t ChimpFinalAnalyze(AnalyzeState &state) {
	throw InternalException("Chimp has been deprecated, can no longer be used to compress data");
}

} // namespace goose
