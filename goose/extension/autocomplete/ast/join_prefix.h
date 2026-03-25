#pragma once
#include <goose/common/enums/join_type.h>
#include <goose/common/enums/joinref_type.h>

namespace goose {
struct JoinPrefix {
	JoinRefType ref_type;
	JoinType join_type;
};
} // namespace goose
