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

#include <goose/common/types/hugeint.h>
#include <goose/common/operator/add.h>
#include <goose/common/operator/multiply.h>
#include <goose/function/aggregate_state.h>
#include <goose/common/operator/cast_operators.h>

namespace goose {

static inline void KahanAddInternal(double input, double &summed, double &err) {
	double diff = input - err;
	double newval = summed + diff;
	err = (newval - summed) - diff;
	summed = newval;
}

template <class T>
struct SumState {
	bool isset;
	T value;

	void Initialize() {
		this->isset = false;
		this->value = 0;
	}

	void Combine(const SumState<T> &other) {
		this->isset = other.isset || this->isset;
		this->value += other.value;
	}
};

struct KahanSumState {
	bool isset;
	double value;
	double err;

	void Initialize() {
		this->isset = false;
		this->err = 0.0;
	}

	void Combine(const KahanSumState &other) {
		this->isset = other.isset || this->isset;
		KahanAddInternal(other.value, this->value, this->err);
		KahanAddInternal(other.err, this->value, this->err);
	}
};

struct RegularAdd {
	template <class STATE, class T>
	static void AddNumber(STATE &state, T input) {
		state.value += input;
	}

	template <class STATE, class T>
	static void AddConstant(STATE &state, T input, idx_t count) {
		state.value += input * int64_t(count);
	}
};

struct HugeintAdd {
	template <class STATE, class T>
	static void AddNumber(STATE &state, T input) {
		state.value = Hugeint::Add(state.value, input);
	}

	template <class STATE, class T>
	static void AddConstant(STATE &state, T input, idx_t count) {
		AddNumber(state, Hugeint::Multiply(input, UnsafeNumericCast<int64_t>(count)));
	}
};

struct IntervalAdd {
	template <class STATE, class T>
	static void AddNumber(STATE &state, T input) {
		state.value = AddOperator::Operation<interval_t, interval_t, interval_t>(state.value, input);
	}

	template <class STATE, class T>
	static void AddConstant(STATE &state, T input, idx_t count) {
		const auto count64 = Cast::Operation<idx_t, int64_t>(count);
		input = MultiplyOperator::Operation<interval_t, int64_t, interval_t>(input, count64);
		state.value = AddOperator::Operation<interval_t, interval_t, interval_t>(state.value, input);
	}
};

struct KahanAdd {
	template <class STATE, class T>
	static void AddNumber(STATE &state, T input) {
		KahanAddInternal(input, state.value, state.err);
	}

	template <class STATE, class T>
	static void AddConstant(STATE &state, T input, idx_t count) {
		KahanAddInternal(input * count, state.value, state.err);
	}
};

struct AddToHugeint {
	static void AddValue(hugeint_t &result, uint64_t value, int positive) {
		// integer summation taken from Tim Gubner et al. - Efficient Query Processing
		// with Optimistically Compressed Hash Tables & Strings in the USSR

		// add the value to the lower part of the hugeint
		result.lower += value;
		// now handle overflows
		int overflow = result.lower < value;
		// we consider two situations:
		// (1) input[idx] is positive, and current value is lower than value: overflow
		// (2) input[idx] is negative, and current value is higher than value: underflow
		if (!(overflow ^ positive)) {
			// in the case of an overflow or underflow we either increment or decrement the upper base
			// positive: +1, negative: -1
			result.upper += -1 + 2 * positive;
		}
	}

	template <class STATE, class T>
	static void AddNumber(STATE &state, T input) {
		AddValue(state.value, uint64_t(input), input >= 0);
	}

	template <class STATE, class T>
	static void AddConstant(STATE &state, T input, idx_t count) {
		// add a constant X number of times
		// fast path: check if value * count fits into a uint64_t
		// note that we check if value * VECTOR_SIZE fits in a uint64_t to avoid having to actually do a division
		// this is still a pretty high number (18014398509481984) so most positive numbers will fit
		if (input >= 0 && uint64_t(input) < (NumericLimits<uint64_t>::Maximum() / STANDARD_VECTOR_SIZE)) {
			// if it does just multiply it and add the value
			uint64_t value = uint64_t(input) * count;
			AddValue(state.value, value, 1);
		} else {
			// if it doesn't fit we have two choices
			// either we loop over count and add the values individually
			// or we convert to a hugeint and multiply the hugeint
			// the problem is that hugeint multiplication is expensive
			// hence we switch here: with a low count we do the loop
			// with a high count we do the hugeint multiplication
			if (count < 8) {
				for (idx_t i = 0; i < count; i++) {
					AddValue(state.value, uint64_t(input), input >= 0);
				}
			} else {
				hugeint_t addition = hugeint_t(input) * Hugeint::Convert(count);
				state.value += addition;
			}
		}
	}
};

template <class STATEOP, class ADDOP>
struct BaseSumOperation {
	template <class STATE>
	static void Initialize(STATE &state) {
		state.value = 0;
		STATEOP::template Initialize<STATE>(state);
	}

	template <class STATE, class OP>
	static void Combine(const STATE &source, STATE &target, AggregateInputData &aggr_input_data) {
		STATEOP::template Combine<STATE>(source, target, aggr_input_data);
	}

	template <class INPUT_TYPE, class STATE, class OP>
	static void Operation(STATE &state, const INPUT_TYPE &input, AggregateUnaryInput &) {
		STATEOP::template AddValues<STATE>(state, 1);
		ADDOP::template AddNumber<STATE, INPUT_TYPE>(state, input);
	}

	template <class INPUT_TYPE, class STATE, class OP>
	static void ConstantOperation(STATE &state, const INPUT_TYPE &input, AggregateUnaryInput &, idx_t count) {
		STATEOP::template AddValues<STATE>(state, count);
		ADDOP::template AddConstant<STATE, INPUT_TYPE>(state, input, count);
	}
	static bool IgnoreNull() {
		return true;
	}
};

} // namespace goose
