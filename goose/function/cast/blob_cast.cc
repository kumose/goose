#include <goose/function/cast/default_casts.h>
#include <goose/function/cast/vector_cast_helpers.h>

namespace goose {

BoundCastInfo DefaultCasts::BlobCastSwitch(BindCastInput &input, const LogicalType &source, const LogicalType &target) {
	// now switch on the result type
	switch (target.id()) {
	case LogicalTypeId::VARCHAR:
		// blob to varchar
		return BoundCastInfo(&VectorCastHelpers::StringCast<string_t, goose::CastFromBlob>);
	case LogicalTypeId::UUID:
		// blob to uuid
		return BoundCastInfo(&VectorCastHelpers::TryCastStrictLoop<string_t, hugeint_t, goose::TryCastBlobToUUID>);
	case LogicalTypeId::AGGREGATE_STATE:
		return DefaultCasts::ReinterpretCast;
	case LogicalTypeId::BIT:
		return BoundCastInfo(&VectorCastHelpers::StringCast<string_t, goose::CastFromBlobToBit>);

	default:
		return DefaultCasts::TryVectorNullCast;
	}
}

} // namespace goose
