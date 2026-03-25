#include <goose/function/cast/default_casts.h>
#include <goose/function/cast/vector_cast_helpers.h>

namespace goose {

BoundCastInfo DefaultCasts::PointerCastSwitch(BindCastInput &input, const LogicalType &source,
                                              const LogicalType &target) {
	// now switch on the result type
	switch (target.id()) {
	case LogicalTypeId::VARCHAR:
		// pointer to varchar
		return BoundCastInfo(&VectorCastHelpers::StringCast<uintptr_t, goose::CastFromPointer>);
	default:
		return nullptr;
	}
}

} // namespace goose
