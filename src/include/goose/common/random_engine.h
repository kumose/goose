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
