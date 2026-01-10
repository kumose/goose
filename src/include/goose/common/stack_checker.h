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

template <class RECURSIVE_CLASS>
class StackChecker {
public:
	StackChecker(RECURSIVE_CLASS &recursive_class_p, idx_t stack_usage_p)
	    : recursive_class(recursive_class_p), stack_usage(stack_usage_p) {
		recursive_class.stack_depth += stack_usage;
	}
	~StackChecker() {
		recursive_class.stack_depth -= stack_usage;
	}
	StackChecker(StackChecker &&other) noexcept
	    : recursive_class(other.recursive_class), stack_usage(other.stack_usage) {
		other.stack_usage = 0;
	}
	StackChecker(const StackChecker &) = delete;

private:
	RECURSIVE_CLASS &recursive_class;
	idx_t stack_usage;
};

} // namespace goose
