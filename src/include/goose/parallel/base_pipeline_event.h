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

#include <goose/parallel/event.h>
#include <goose/parallel/pipeline.h>

namespace goose {

//! A BasePipelineEvent is used as the basis of any event that belongs to a specific pipeline
class BasePipelineEvent : public Event {
public:
	explicit BasePipelineEvent(shared_ptr<Pipeline> pipeline);
	explicit BasePipelineEvent(Pipeline &pipeline);

	void PrintPipeline() override {
		pipeline->Print();
	}

	//! The pipeline that this event belongs to
	shared_ptr<Pipeline> pipeline;
};

} // namespace goose
