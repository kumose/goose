#include <goose/execution/operator/join/physical_delim_join.h>

#include <goose/execution/operator/aggregate/physical_hash_aggregate.h>

namespace goose {

PhysicalDelimJoin::PhysicalDelimJoin(PhysicalPlan &physical_plan, PhysicalOperatorType type, vector<LogicalType> types,
                                     PhysicalOperator &original_join, PhysicalOperator &distinct,
                                     const vector<const_reference<PhysicalOperator>> &delim_scans,
                                     idx_t estimated_cardinality, optional_idx delim_idx)
    : PhysicalOperator(physical_plan, type, std::move(types), estimated_cardinality), join(original_join),
      distinct(distinct.Cast<PhysicalHashAggregate>()), delim_scans(delim_scans), delim_idx(delim_idx) {
	D_ASSERT(type == PhysicalOperatorType::LEFT_DELIM_JOIN || type == PhysicalOperatorType::RIGHT_DELIM_JOIN);
}

vector<const_reference<PhysicalOperator>> PhysicalDelimJoin::GetChildren() const {
	vector<const_reference<PhysicalOperator>> result;
	for (auto &child : children) {
		result.push_back(child.get());
	}
	result.push_back(join);
	result.push_back(distinct);
	return result;
}

InsertionOrderPreservingMap<string> PhysicalDelimJoin::ParamsToString() const {
	auto result = join.ParamsToString();
	result["Delim Index"] = StringUtil::Format("%llu", delim_idx.GetIndex());
	return result;
}

} // namespace goose
