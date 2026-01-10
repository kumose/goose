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

#include <goose/common/assert.h>

namespace goose {

struct ProgressData {
	double done = 0.0;
	double total = 0.0;
	bool invalid = false;

public:
	double ProgressDone() const {
		// ProgressDone requires a valid state
		D_ASSERT(IsValid());

		return done / total;
	}

	void Add(const ProgressData &other) {
		// Add is unchecked, propagating invalid
		done += other.done;
		total += other.total;
		invalid = invalid || other.invalid;
	}
	void Normalize(const double target = 1.0) {
		// Normalize checks only `target`, propagating invalid
		D_ASSERT(target > 0.0);
		if (IsValid()) {
			if (total > 0.0) {
				done /= total;
			}
			total = 1.0;
			done *= target;
			total *= target;
		} else {
			SetInvalid();
		}
	}
	void SetInvalid() {
		invalid = true;
		done = 0.0;
		total = 1.0;
	}
	bool IsValid() const {
		return (!invalid) && (done >= 0.0) && (done <= total) && (total >= 0.0);
	}
};

} // namespace goose
