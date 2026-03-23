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

#include <goose/catalog/catalog.h>
#include <goose/common/common.h>
#include <goose/common/enums/operator_result_type.h>
#include <goose/common/enums/physical_operator_type.h>
#include <goose/common/types/data_chunk.h>
#include <goose/execution/execution_context.h>
#include <goose/optimizer/join_order/join_node.h>
#include <goose/parallel/interrupt.h>
#include <goose/execution/partition_info.h>

namespace goose {
class Event;
class Executor;
class PhysicalOperator;
class Pipeline;
class PipelineBuildState;
class MetaPipeline;
class InterruptState;

// LCOV_EXCL_START
class OperatorState {
public:
	virtual ~OperatorState() {
	}

	virtual void Finalize(const PhysicalOperator &op, ExecutionContext &context) {
	}

	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}
};

class GlobalOperatorState {
public:
	virtual ~GlobalOperatorState() {
	}

	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}

	virtual idx_t MaxThreads(idx_t source_max_threads) {
		return source_max_threads;
	}
};

class GlobalSinkState : public StateWithBlockableTasks {
public:
	GlobalSinkState() : state(SinkFinalizeType::READY) {
	}
	virtual ~GlobalSinkState() {
	}

	SinkFinalizeType state;

	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}

	virtual idx_t MaxThreads(idx_t source_max_threads) {
		return source_max_threads;
	}
};

class LocalSinkState {
public:
	virtual ~LocalSinkState() {
	}

	//! Source partition info
	SourcePartitionInfo partition_info;

	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}
};

class GlobalSourceState : public StateWithBlockableTasks {
public:
	virtual ~GlobalSourceState() {
	}

	virtual idx_t MaxThreads() {
		return 1;
	}

	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}
};

class LocalSourceState {
public:
	virtual ~LocalSourceState() {
	}

	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}
};

struct OperatorSinkInput {
	GlobalSinkState &global_state;
	LocalSinkState &local_state;
	InterruptState &interrupt_state;
};

struct OperatorSourceInput {
	GlobalSourceState &global_state;
	LocalSourceState &local_state;
	InterruptState &interrupt_state;
};

struct OperatorSinkCombineInput {
	GlobalSinkState &global_state;
	LocalSinkState &local_state;
	InterruptState &interrupt_state;
};

struct OperatorSinkFinalizeInput {
	GlobalSinkState &global_state;
	InterruptState &interrupt_state;
};

struct OperatorFinalizeInput {
	GlobalOperatorState &global_state;
	InterruptState &interrupt_state;
};

struct OperatorSinkNextBatchInput {
	GlobalSinkState &global_state;
	LocalSinkState &local_state;
	InterruptState &interrupt_state;
};

// LCOV_EXCL_STOP

} // namespace goose
