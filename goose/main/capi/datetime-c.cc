#include <goose/main/capi/capi_internal.h>
#include <goose/common/numeric_utils.h>
#include <goose/common/types/date.h>
#include <goose/common/types/time.h>
#include <goose/common/types/timestamp.h>

using goose::Date;
using goose::Time;
using goose::Timestamp;

using goose::date_t;
using goose::dtime_t;
using goose::timestamp_ms_t;
using goose::timestamp_ns_t;
using goose::timestamp_sec_t;
using goose::timestamp_t;

goose_date_struct goose_from_date(goose_date date) {
	int32_t year, month, day;
	Date::Convert(date_t(date.days), year, month, day);

	goose_date_struct result;
	result.year = year;
	result.month = goose::UnsafeNumericCast<int8_t>(month);
	result.day = goose::UnsafeNumericCast<int8_t>(day);
	return result;
}

goose_date goose_to_date(goose_date_struct date) {
	goose_date result;
	result.days = Date::FromDate(date.year, date.month, date.day).days;
	return result;
}

bool goose_is_finite_date(goose_date date) {
	return Date::IsFinite(date_t(date.days));
}

goose_time_struct goose_from_time(goose_time time) {
	int32_t hour, minute, second, micros;
	Time::Convert(dtime_t(time.micros), hour, minute, second, micros);

	goose_time_struct result;
	result.hour = goose::UnsafeNumericCast<int8_t>(hour);
	result.min = goose::UnsafeNumericCast<int8_t>(minute);
	result.sec = goose::UnsafeNumericCast<int8_t>(second);
	result.micros = micros;
	return result;
}

goose_time_tz_struct goose_from_time_tz(goose_time_tz input) {
	goose_time_tz_struct result;
	goose_time time;

	goose::dtime_tz_t time_tz(input.bits);

	time.micros = time_tz.time().micros;

	result.time = goose_from_time(time);
	result.offset = time_tz.offset();
	return result;
}

goose_time_tz goose_create_time_tz(int64_t micros, int32_t offset) {
	goose_time_tz time;
	time.bits = goose::dtime_tz_t(goose::dtime_t(micros), offset).bits;
	return time;
}

goose_time goose_to_time(goose_time_struct time) {
	goose_time result;
	result.micros = Time::FromTime(time.hour, time.min, time.sec, time.micros).micros;
	return result;
}

goose_timestamp_struct goose_from_timestamp(goose_timestamp ts) {
	date_t date;
	dtime_t time;
	Timestamp::Convert(timestamp_t(ts.micros), date, time);

	goose_date ddate;
	ddate.days = date.days;

	goose_time dtime;
	dtime.micros = time.micros;

	goose_timestamp_struct result;
	result.date = goose_from_date(ddate);
	result.time = goose_from_time(dtime);
	return result;
}

goose_timestamp goose_to_timestamp(goose_timestamp_struct ts) {
	date_t date = date_t(goose_to_date(ts.date).days);
	dtime_t time = dtime_t(goose_to_time(ts.time).micros);

	goose_timestamp result;
	result.micros = Timestamp::FromDatetime(date, time).value;
	return result;
}

bool goose_is_finite_timestamp(goose_timestamp ts) {
	return Timestamp::IsFinite(timestamp_t(ts.micros));
}

bool goose_is_finite_timestamp_s(goose_timestamp_s ts) {
	return Timestamp::IsFinite(timestamp_sec_t(ts.seconds));
}

bool goose_is_finite_timestamp_ms(goose_timestamp_ms ts) {
	return Timestamp::IsFinite(timestamp_ms_t(ts.millis));
}

bool goose_is_finite_timestamp_ns(goose_timestamp_ns ts) {
	return Timestamp::IsFinite(timestamp_ns_t(ts.nanos));
}
