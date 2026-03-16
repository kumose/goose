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
