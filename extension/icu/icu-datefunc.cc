#include "include/icu-datefunc.h"

#include <goose/main/client_context.h>
#include <goose/common/operator/add.h>
#include <goose/common/operator/multiply.h>
#include <goose/common/types/timestamp.h>
#include <goose/common/exception/conversion_exception.h>
#include "icu-helpers.h"
#include <xicu/unicode/ucal.h>

namespace goose {
    ICUDateFunc::BindData::BindData(const BindData &other)
        : tz_setting(other.tz_setting), cal_setting(other.cal_setting), calendar(other.calendar->clone()) {
    }

    ICUDateFunc::BindData::BindData(const string &tz_setting_p, const string &cal_setting_p)
        : tz_setting(tz_setting_p), cal_setting(cal_setting_p) {
        InitCalendar();
    }

    ICUDateFunc::BindData::BindData(ClientContext &context) {
        Value tz_value;
        if (context.TryGetCurrentSetting("TimeZone", tz_value)) {
            tz_setting = tz_value.ToString();
        }

        Value cal_value;
        if (context.TryGetCurrentSetting("Calendar", cal_value)) {
            cal_setting = cal_value.ToString();
        } else {
            cal_setting = "gregorian";
        }

        InitCalendar();
    }

    void ICUDateFunc::BindData::InitCalendar() {
        auto tz = xicu::TimeZone::createTimeZone(xicu::UnicodeString::fromUTF8(xicu::StringPiece(tz_setting)));

        string cal_id("@calendar=");
        cal_id += cal_setting;

        xicu::Locale locale(cal_id.c_str());

        UErrorCode success = U_ZERO_ERROR;
        calendar.reset(xicu::Calendar::createInstance(tz, locale, success));
        if (U_FAILURE(success)) {
            throw InternalException("Unable to create ICU calendar.");
        }

        //	Postgres always assumes times are given in the proleptic Gregorian calendar.
        //	ICU defaults to the Gregorian change in 1582, so we reset the change to the minimum date
        //	so that all dates are proleptic Gregorian.
        //	The only error here is if we have a non-Gregorian calendar,
        //	and we just ignore that and hope for the best...
        ucal_setGregorianChange((UCalendar *) calendar.get(), U_DATE_MIN, &success); // NOLINT
    }

    bool ICUDateFunc::BindData::Equals(const FunctionData &other_p) const {
        auto &other = other_p.Cast<const BindData>();
        return calendar->isEquivalentTo(*other.calendar);
    }

    unique_ptr<FunctionData> ICUDateFunc::BindData::Copy() const {
        return make_uniq<BindData>(*this);
    }

    unique_ptr<FunctionData> ICUDateFunc::Bind(ClientContext &context, ScalarFunction &bound_function,
                                               vector<goose::unique_ptr<Expression> > &arguments) {
        return make_uniq<BindData>(context);
    }

    bool ICUDateFunc::TrySetTimeZone(xicu::Calendar *calendar, const string_t &tz_id) {
        string tz_str = tz_id.GetString();
        auto tz = ICUHelpers::TryGetTimeZone(tz_str);
        if (!tz) {
            return false;
        }
        calendar->adoptTimeZone(tz.release());
        return true;
    }

    void ICUDateFunc::SetTimeZone(xicu::Calendar *calendar, const string_t &tz_id, string *error_message) {
        string tz_str = tz_id.GetString();
        auto tz = ICUHelpers::GetTimeZone(tz_str, error_message);
        if (tz) {
            calendar->adoptTimeZone(tz.release());
        }
    }

    timestamp_t ICUDateFunc::GetTimeUnsafe(xicu::Calendar *calendar, uint64_t micros) {
        // Extract the new time
        UErrorCode status = U_ZERO_ERROR;
        const auto millis = int64_t(calendar->getTime(status));
        if (U_FAILURE(status)) {
            throw InternalException("Unable to get ICU calendar time.");
        }
        return timestamp_t(millis * Interval::MICROS_PER_MSEC + int64_t(micros));
    }

    bool ICUDateFunc::TryGetTime(xicu::Calendar *calendar, uint64_t micros, timestamp_t &result) {
        // Extract the new time
        UErrorCode status = U_ZERO_ERROR;
        auto millis = int64_t(calendar->getTime(status));
        if (U_FAILURE(status)) {
            return false;
        }

        // UDate is a double, so it can't overflow (it just loses accuracy), but converting back to µs can.
        if (!TryMultiplyOperator::Operation<int64_t, int64_t, int64_t>(millis, Interval::MICROS_PER_MSEC, millis)) {
            return false;
        }
        if (!TryAddOperator::Operation<int64_t, int64_t, int64_t>(millis, int64_t(micros), millis)) {
            return false;
        }

        // Now make sure the value is in range
        result = timestamp_t(millis);
        date_t out_date = Timestamp::GetDate(result);
        int64_t days_micros;
        return TryMultiplyOperator::Operation<int64_t, int64_t, int64_t>(out_date.days, Interval::MICROS_PER_DAY,
                                                                         days_micros);
    }

    timestamp_t ICUDateFunc::GetTime(xicu::Calendar *calendar, uint64_t micros) {
        timestamp_t result;
        if (!TryGetTime(calendar, micros, result)) {
            throw ConversionException("ICU date overflows timestamp range");
        }
        return result;
    }

    uint64_t ICUDateFunc::SetTime(xicu::Calendar *calendar, timestamp_t date) {
        int64_t millis = date.value / Interval::MICROS_PER_MSEC;
        int64_t micros = date.value % Interval::MICROS_PER_MSEC;
        if (micros < 0) {
            --millis;
            micros += Interval::MICROS_PER_MSEC;
        }

        const auto udate = UDate(millis);
        UErrorCode status = U_ZERO_ERROR;
        calendar->setTime(udate, status);
        if (U_FAILURE(status)) {
            throw InternalException("Unable to set ICU calendar time.");
        }
        return uint64_t(micros);
    }

    int32_t ICUDateFunc::ExtractField(xicu::Calendar *calendar, UCalendarDateFields field) {
        UErrorCode status = U_ZERO_ERROR;
        const auto result = calendar->get(field, status);
        if (U_FAILURE(status)) {
            throw InternalException("Unable to extract ICU calendar part.");
        }
        return result;
    }

    int32_t ICUDateFunc::SubtractField(xicu::Calendar *calendar, UCalendarDateFields field, timestamp_t end_date) {
        const int64_t millis = end_date.value / Interval::MICROS_PER_MSEC;
        const auto when = UDate(millis);
        UErrorCode status = U_ZERO_ERROR;
        auto sub = calendar->fieldDifference(when, field, status);
        if (U_FAILURE(status)) {
            throw InternalException("Unable to subtract ICU calendar part.");
        }
        return sub;
    }
} // namespace goose
