#pragma once

#include <cstdint>

namespace goose {

enum class ArrowTypeInfoType : uint8_t { LIST, STRUCT, DATE_TIME, STRING, ARRAY, DECIMAL };

} // namespace goose
