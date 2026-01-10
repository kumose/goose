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

namespace goose {

struct PickLeft {
	template <class T>
	static inline T Operation(T left, T right) {
		return left;
	}
};

struct PickRight {
	template <class T>
	static inline T Operation(T left, T right) {
		return right;
	}
};

struct NOP {
	template <class T>
	static inline T Operation(T left) {
		return left;
	}
};

struct ConstantZero {
	template <class T>
	static inline T Operation(T left, T right) {
		return 0;
	}
};

struct ConstantOne {
	template <class T>
	static inline T Operation(T left, T right) {
		return 1;
	}
};

struct AddOne {
	template <class T>
	static inline T Operation(T left, T right) {
		return right + 1;
	}
};

} // namespace goose
