#include <goose/function/cast/default_casts.h>
#include <goose/function/cast/vector_cast_helpers.h>
#include <goose/common/operator/string_cast.h>
namespace goose {

BoundCastInfo DefaultCasts::DateCastSwitch(BindCastInput &input, const LogicalType &source, const LogicalType &target) {
	// now switch on the result type
	switch (target.id()) {
	case LogicalTypeId::VARCHAR:
		// date to varchar
		return BoundCastInfo(&VectorCastHelpers::StringCast<date_t, goose::StringCast>);
	case LogicalTypeId::TIMESTAMP:
	case LogicalTypeId::TIMESTAMP_TZ:
		// date to timestamp
		return BoundCastInfo(&VectorCastHelpers::TryCastLoop<date_t, timestamp_t, goose::TryCast>);
	case LogicalTypeId::TIMESTAMP_NS:
		return BoundCastInfo(&VectorCastHelpers::TryCastLoop<date_t, timestamp_ns_t, goose::TryCastToTimestampNS>);
	case LogicalTypeId::TIMESTAMP_SEC:
		return BoundCastInfo(&VectorCastHelpers::TryCastLoop<date_t, timestamp_t, goose::TryCastToTimestampSec>);
	case LogicalTypeId::TIMESTAMP_MS:
		return BoundCastInfo(&VectorCastHelpers::TryCastLoop<date_t, timestamp_t, goose::TryCastToTimestampMS>);
	default:
		return TryVectorNullCast;
	}
}

BoundCastInfo DefaultCasts::TimeCastSwitch(BindCastInput &input, const LogicalType &source, const LogicalType &target) {
	// now switch on the result type
	switch (target.id()) {
	case LogicalTypeId::VARCHAR:
		// time to varchar
		return BoundCastInfo(&VectorCastHelpers::StringCast<dtime_t, goose::StringCast>);
	case LogicalTypeId::TIME_TZ:
		// time to time with time zone
		return BoundCastInfo(&VectorCastHelpers::TemplatedCastLoop<dtime_t, dtime_tz_t, goose::Cast>);
	default:
		return TryVectorNullCast;
	}
}

BoundCastInfo DefaultCasts::TimeNsCastSwitch(BindCastInput &input, const LogicalType &src, const LogicalType &target) {
	// now switch on the result type
	switch (target.id()) {
	case LogicalTypeId::VARCHAR:
		// time (ns) to varchar
		return BoundCastInfo(&VectorCastHelpers::StringCast<dtime_ns_t, goose::StringCast>);
	case LogicalTypeId::TIME:
		// time (ns) to time (µs)
		return BoundCastInfo(&VectorCastHelpers::TemplatedCastLoop<dtime_ns_t, dtime_t, goose::Cast>);
	default:
		return TryVectorNullCast;
	}
}

BoundCastInfo DefaultCasts::TimeTzCastSwitch(BindCastInput &input, const LogicalType &source,
                                             const LogicalType &target) {
	// now switch on the result type
	switch (target.id()) {
	case LogicalTypeId::VARCHAR:
		// time with time zone to varchar
		return BoundCastInfo(&VectorCastHelpers::StringCast<dtime_tz_t, goose::StringCastTZ>);
	case LogicalTypeId::TIME:
		// time with time zone to time
		return BoundCastInfo(&VectorCastHelpers::TemplatedCastLoop<dtime_tz_t, dtime_t, goose::Cast>);
	default:
		return TryVectorNullCast;
	}
}

BoundCastInfo DefaultCasts::TimestampCastSwitch(BindCastInput &input, const LogicalType &source,
                                                const LogicalType &target) {
	// now switch on the result type
	switch (target.id()) {
	case LogicalTypeId::VARCHAR:
		// timestamp to varchar
		return BoundCastInfo(&VectorCastHelpers::StringCast<timestamp_t, goose::StringCast>);
	case LogicalTypeId::DATE:
		// timestamp to date
		return BoundCastInfo(&VectorCastHelpers::TemplatedCastLoop<timestamp_t, date_t, goose::Cast>);
	case LogicalTypeId::TIME:
		// timestamp to time
		return BoundCastInfo(&VectorCastHelpers::TemplatedCastLoop<timestamp_t, dtime_t, goose::Cast>);
	case LogicalTypeId::TIME_TZ:
		// timestamp to time_tz
		return BoundCastInfo(&VectorCastHelpers::TemplatedCastLoop<timestamp_t, dtime_tz_t, goose::Cast>);
	case LogicalTypeId::TIMESTAMP_TZ:
		// timestamp (us) to timestamp with time zone
		return ReinterpretCast;
	case LogicalTypeId::TIMESTAMP_NS:
		// timestamp (us) to timestamp (ns)
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, timestamp_t, goose::CastTimestampUsToNs>);
	case LogicalTypeId::TIMESTAMP_MS:
		// timestamp (us) to timestamp (ms)
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, timestamp_t, goose::CastTimestampUsToMs>);
	case LogicalTypeId::TIMESTAMP_SEC:
		// timestamp (us) to timestamp (s)
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, timestamp_t, goose::CastTimestampUsToSec>);
	default:
		return TryVectorNullCast;
	}
}

BoundCastInfo DefaultCasts::TimestampTzCastSwitch(BindCastInput &input, const LogicalType &source,
                                                  const LogicalType &target) {
	// now switch on the result type
	switch (target.id()) {
	case LogicalTypeId::VARCHAR:
		// timestamp with time zone to varchar
		return BoundCastInfo(&VectorCastHelpers::StringCast<timestamp_t, goose::StringCastTZ>);
	case LogicalTypeId::TIME_TZ:
		// timestamp with time zone to time with time zone.
		return BoundCastInfo(&VectorCastHelpers::TemplatedCastLoop<timestamp_t, dtime_tz_t, goose::Cast>);
	case LogicalTypeId::TIMESTAMP:
		// timestamp with time zone to timestamp (us)
		return ReinterpretCast;
	default:
		return TryVectorNullCast;
	}
}

BoundCastInfo DefaultCasts::TimestampNsCastSwitch(BindCastInput &input, const LogicalType &source,
                                                  const LogicalType &target) {
	// now switch on the result type
	switch (target.id()) {
	case LogicalTypeId::VARCHAR:
		// timestamp (ns) to varchar
		return BoundCastInfo(&VectorCastHelpers::StringCast<timestamp_ns_t, goose::StringCast>);
	case LogicalTypeId::DATE:
		// timestamp (ns) to date
		return BoundCastInfo(&VectorCastHelpers::TemplatedCastLoop<timestamp_t, date_t, goose::CastTimestampNsToDate>);
	case LogicalTypeId::TIME:
		// timestamp (ns) to time
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, dtime_t, goose::CastTimestampNsToTime>);
	case LogicalTypeId::TIME_NS:
		// timestamp (ns) to time (ns)
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_ns_t, dtime_ns_t, goose::CastTimestampNsToTimeNs>);
	case LogicalTypeId::TIMESTAMP:
		// timestamp (ns) to timestamp (us)
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, timestamp_t, goose::CastTimestampNsToUs>);
	case LogicalTypeId::TIMESTAMP_TZ:
		// timestamp (ns) to timestamp with time zone (us)
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, timestamp_t, goose::CastTimestampNsToUs>);
	default:
		return TryVectorNullCast;
	}
}

BoundCastInfo DefaultCasts::TimestampMsCastSwitch(BindCastInput &input, const LogicalType &source,
                                                  const LogicalType &target) {
	// now switch on the result type
	switch (target.id()) {
	case LogicalTypeId::VARCHAR:
		// timestamp (ms) to varchar
		return BoundCastInfo(&VectorCastHelpers::StringCast<timestamp_t, goose::CastFromTimestampMS>);
	case LogicalTypeId::DATE:
		// timestamp (ms) to date
		return BoundCastInfo(&VectorCastHelpers::TemplatedCastLoop<timestamp_t, date_t, goose::CastTimestampMsToDate>);
	case LogicalTypeId::TIME:
		// timestamp (ms) to time
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, dtime_t, goose::CastTimestampMsToTime>);
	case LogicalTypeId::TIMESTAMP:
		// timestamp (ms) to timestamp (us)
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, timestamp_t, goose::CastTimestampMsToUs>);
	case LogicalTypeId::TIMESTAMP_NS:
		// timestamp (ms) to timestamp (ns)
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, timestamp_t, goose::CastTimestampMsToNs>);
	case LogicalTypeId::TIMESTAMP_TZ:
		// timestamp (ms) to timestamp with timezone (us)
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, timestamp_t, goose::CastTimestampMsToUs>);
	default:
		return TryVectorNullCast;
	}
}

BoundCastInfo DefaultCasts::TimestampSecCastSwitch(BindCastInput &input, const LogicalType &source,
                                                   const LogicalType &target) {
	// now switch on the result type
	switch (target.id()) {
	case LogicalTypeId::VARCHAR:
		// timestamp (s) to varchar
		return BoundCastInfo(&VectorCastHelpers::StringCast<timestamp_t, goose::CastFromTimestampSec>);
	case LogicalTypeId::DATE:
		// timestamp (s) to date
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, date_t, goose::CastTimestampSecToDate>);
	case LogicalTypeId::TIME:
		// timestamp (s) to time
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, dtime_t, goose::CastTimestampSecToTime>);
	case LogicalTypeId::TIMESTAMP_MS:
		// timestamp (s) to timestamp (ms)
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, timestamp_t, goose::CastTimestampSecToMs>);
	case LogicalTypeId::TIMESTAMP:
		// timestamp (s) to timestamp (us)
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, timestamp_t, goose::CastTimestampSecToUs>);
	case LogicalTypeId::TIMESTAMP_TZ:
		// timestamp (s) to timestamp with timezone (us)
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, timestamp_t, goose::CastTimestampSecToUs>);
	case LogicalTypeId::TIMESTAMP_NS:
		// timestamp (s) to timestamp (ns)
		return BoundCastInfo(
		    &VectorCastHelpers::TemplatedCastLoop<timestamp_t, timestamp_t, goose::CastTimestampSecToNs>);
	default:
		return TryVectorNullCast;
	}
}
BoundCastInfo DefaultCasts::IntervalCastSwitch(BindCastInput &input, const LogicalType &source,
                                               const LogicalType &target) {
	// now switch on the result type
	switch (target.id()) {
	case LogicalTypeId::VARCHAR:
		// time to varchar
		return BoundCastInfo(&VectorCastHelpers::StringCast<interval_t, goose::StringCast>);
	default:
		return TryVectorNullCast;
	}
}

} // namespace goose
