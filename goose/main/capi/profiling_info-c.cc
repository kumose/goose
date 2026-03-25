#include <goose/main/capi/capi_internal.h>

using goose::Connection;
using goose::Goose;
using goose::EnumUtil;
using goose::MetricType;
using goose::optional_ptr;
using goose::ProfilingNode;

goose_profiling_info goose_get_profiling_info(goose_connection connection) {
	if (!connection) {
		return nullptr;
	}
	Connection *conn = reinterpret_cast<Connection *>(connection);
	optional_ptr<ProfilingNode> profiling_node;
	try {
		profiling_node = conn->GetProfilingTree();
	} catch (std::exception &ex) {
		return nullptr;
	}

	if (!profiling_node) {
		return nullptr;
	}
	return reinterpret_cast<goose_profiling_info>(profiling_node.get());
}

goose_value goose_profiling_info_get_value(goose_profiling_info info, const char *key) {
	if (!info) {
		return nullptr;
	}
	auto &node = *reinterpret_cast<goose::ProfilingNode *>(info);
	auto &profiling_info = node.GetProfilingInfo();
	auto key_enum = EnumUtil::FromString<MetricType>(goose::StringUtil::Upper(key));
	if (!profiling_info.Enabled(profiling_info.settings, key_enum)) {
		return nullptr;
	}

	auto str = profiling_info.GetMetricAsString(key_enum);
	return goose_create_varchar_length(str.c_str(), strlen(str.c_str()));
}

goose_value goose_profiling_info_get_metrics(goose_profiling_info info) {
	if (!info) {
		return nullptr;
	}

	auto &node = *reinterpret_cast<goose::ProfilingNode *>(info);
	auto &profiling_info = node.GetProfilingInfo();

	goose::InsertionOrderPreservingMap<goose::string> metrics_map;
	for (const auto &metric : profiling_info.metrics) {
		auto key = EnumUtil::ToString(metric.first);
		if (!profiling_info.Enabled(profiling_info.settings, metric.first)) {
			continue;
		}

		if (key == EnumUtil::ToString(MetricType::OPERATOR_TYPE)) {
			auto type = goose::PhysicalOperatorType(metric.second.GetValue<uint8_t>());
			metrics_map[key] = EnumUtil::ToString(type);
		} else {
			metrics_map[key] = metric.second.ToString();
		}
	}

	auto map = goose::Value::MAP(metrics_map);
	return reinterpret_cast<goose_value>(new goose::Value(map));
}

idx_t goose_profiling_info_get_child_count(goose_profiling_info info) {
	if (!info) {
		return 0;
	}
	auto &node = *reinterpret_cast<goose::ProfilingNode *>(info);
	return node.GetChildCount();
}

goose_profiling_info goose_profiling_info_get_child(goose_profiling_info info, idx_t index) {
	if (!info) {
		return nullptr;
	}
	auto &node = *reinterpret_cast<goose::ProfilingNode *>(info);
	if (index >= node.GetChildCount()) {
		return nullptr;
	}

	ProfilingNode *profiling_info_ptr = node.GetChild(index).get();
	return reinterpret_cast<goose_profiling_info>(profiling_info_ptr);
}
