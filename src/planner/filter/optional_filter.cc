#include <goose/planner/table_filter.h>
#include <goose/planner/filter/optional_filter.h>
#include <goose/planner/expression.h>

namespace goose {

OptionalFilter::OptionalFilter(unique_ptr<TableFilter> filter)
    : TableFilter(TableFilterType::OPTIONAL_FILTER), child_filter(std::move(filter)) {
}

FilterPropagateResult OptionalFilter::CheckStatistics(BaseStatistics &stats) const {
	return child_filter->CheckStatistics(stats);
}

string OptionalFilter::ToString(const string &column_name) const {
	return string("optional: ") + child_filter->ToString(column_name);
}

unique_ptr<Expression> OptionalFilter::ToExpression(const Expression &column) const {
	return child_filter->ToExpression(column);
}

idx_t OptionalFilter::FilterSelection(SelectionVector &sel, Vector &vector, UnifiedVectorFormat &vdata,
                                      TableFilterState &filter_state, const idx_t scan_count,
                                      idx_t &approved_tuple_count) const {
	return scan_count;
}

unique_ptr<TableFilter> OptionalFilter::Copy() const {
	auto copy = make_uniq<OptionalFilter>();
	copy->child_filter = child_filter->Copy();
	return goose::unique_ptr_cast<OptionalFilter, TableFilter>(std::move(copy));
}

} // namespace goose
