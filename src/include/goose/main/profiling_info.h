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
#include <goose/common/enums/output_type.h>
#include <goose/common/enums/profiler_format.h>
#include <goose/common/progress_bar/progress_bar.h>
#include <goose/common/types/value.h>
#include <goose/common/types-import.h>
#include <goose/common/constants.h>
#include <goose/common/enums/metric_type.h>

namespace goose_yyjson {
struct yyjson_mut_doc;
struct yyjson_mut_val;
} // namespace goose_yyjson

namespace goose {
enum class ProfilingParameterNames : uint8_t { FORMAT, COVERAGE, SAVE_LOCATION, MODE, METRICS };

class ProfilingInfo {
public:
	//! Enabling a metric adds it to this set.
	profiler_settings_t settings;
	//! This set contains the expanded to-be-collected metrics, which can differ from 'settings'.
	profiler_settings_t expanded_settings;
	//! Contains all enabled metrics.
	profiler_metrics_t metrics;

public:
	ProfilingInfo() = default;
	explicit ProfilingInfo(const profiler_settings_t &n_settings, const idx_t depth = 0);
	ProfilingInfo(ProfilingInfo &) = default;
	ProfilingInfo &operator=(ProfilingInfo const &) = default;

public:
	void ResetMetrics();
	//! Returns true, if the query profiler must collect this metric.
	static bool Enabled(const profiler_settings_t &settings, const MetricType metric);
	//! Expand metrics depending on the collection of other metrics.
	static void Expand(profiler_settings_t &settings, const MetricType metric);

public:
	string GetMetricAsString(const MetricType metric) const;
	void WriteMetricsToLog(ClientContext &context);
	void WriteMetricsToJSON(goose_yyjson::yyjson_mut_doc *doc, goose_yyjson::yyjson_mut_val *destination);

public:
	template <class METRIC_TYPE>
	METRIC_TYPE GetMetricValue(const MetricType type) const {
		auto val = metrics.at(type);
		return val.GetValue<METRIC_TYPE>();
	}

	template <class METRIC_TYPE>
	void MetricUpdate(const MetricType type, const Value &value,
	                  const std::function<METRIC_TYPE(const METRIC_TYPE &, const METRIC_TYPE &)> &update_fun) {
		if (metrics.find(type) == metrics.end()) {
			metrics[type] = value;
			return;
		}
		auto new_value = update_fun(metrics[type].GetValue<METRIC_TYPE>(), value.GetValue<METRIC_TYPE>());
		metrics[type] = Value::CreateValue(new_value);
	}

	template <class METRIC_TYPE>
	void MetricUpdate(const MetricType type, const METRIC_TYPE &value,
	                  const std::function<METRIC_TYPE(const METRIC_TYPE &, const METRIC_TYPE &)> &update_fun) {
		auto new_value = Value::CreateValue(value);
		MetricUpdate<METRIC_TYPE>(type, new_value, update_fun);
	}

	template <class METRIC_TYPE>
	void MetricSum(const MetricType type, const Value &value) {
		MetricUpdate<METRIC_TYPE>(type, value, [](const METRIC_TYPE &old_value, const METRIC_TYPE &new_value) {
			return old_value + new_value;
		});
	}

	template <class METRIC_TYPE>
	void MetricSum(const MetricType type, const METRIC_TYPE &value) {
		auto new_value = Value::CreateValue(value);
		return MetricSum<METRIC_TYPE>(type, new_value);
	}

	template <class METRIC_TYPE>
	void MetricMax(const MetricType type, const Value &value) {
		MetricUpdate<METRIC_TYPE>(type, value, [](const METRIC_TYPE &old_value, const METRIC_TYPE &new_value) {
			return MaxValue(old_value, new_value);
		});
	}

	template <class METRIC_TYPE>
	void MetricMax(const MetricType type, const METRIC_TYPE &value) {
		auto new_value = Value::CreateValue(value);
		return MetricMax<METRIC_TYPE>(type, new_value);
	}
};

// Specialization for InsertionOrderPreservingMap<string>
template <>
inline InsertionOrderPreservingMap<string>
ProfilingInfo::GetMetricValue<InsertionOrderPreservingMap<string>>(const MetricType type) const {
	auto val = metrics.at(type);
	InsertionOrderPreservingMap<string> result;
	auto children = MapValue::GetChildren(val);
	for (auto &child : children) {
		auto struct_children = StructValue::GetChildren(child);
		auto key = struct_children[0].GetValue<string>();
		auto value = struct_children[1].GetValue<string>();
		result.insert(key, value);
	}
	return result;
}
} // namespace goose
