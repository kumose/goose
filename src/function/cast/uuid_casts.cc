#include <goose/function/cast/default_casts.h>
#include <goose/common/operator/cast_operators.h>
#include <goose/function/cast/vector_cast_helpers.h>

namespace goose {

BoundCastInfo DefaultCasts::UUIDCastSwitch(BindCastInput &input, const LogicalType &source, const LogicalType &target) {
	// now switch on the result type
	switch (target.id()) {
	case LogicalTypeId::VARCHAR:
		// uuid to varchar
		return BoundCastInfo(&VectorCastHelpers::StringCast<hugeint_t, goose::CastFromUUID>);
	case LogicalTypeId::BLOB:
		// uuid to blob
		return BoundCastInfo(&VectorCastHelpers::StringCast<hugeint_t, goose::CastFromUUIDToBlob>);
	default:
		return TryVectorNullCast;
	}
}

} // namespace goose
