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

#include <goose/common/common.h>
#include <goose/common/limits.h>
#include <goose/common/types-import.h>

#include <random>

namespace goose {
    class ClientContext;
    struct RandomState;

    class RandomEngine {
    public:
        explicit RandomEngine(int64_t seed = -1);

        ~RandomEngine();

        //! Generate a random number between min and max
        double NextRandom(double min, double max);

        //! Generate a random number between 0 and 1
        double NextRandom();

        //! Generate a random number between 0 and 1, using 32-bits as a base
        double NextRandom32();

        double NextRandom32(double min, double max);

        uint32_t NextRandomInteger32(uint32_t min, uint32_t max);

        uint32_t NextRandomInteger();

        uint32_t NextRandomInteger(uint32_t min, uint32_t max);

        uint64_t NextRandomInteger64();

        void SetSeed(uint64_t seed);

        void RandomData(goose::data_ptr_t data, goose::idx_t len);

        static RandomEngine &Get(ClientContext &context);

        mutex lock;

    private:
        unique_ptr<RandomState> random_state;
    };
} // namespace goose
