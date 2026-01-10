#include <goose/planner/expression/bound_columnref_expression.h>

#include <goose/common/types/hash.h>
#include <goose/main/config.h>

namespace goose {

BoundColumnRefExpression::BoundColumnRefExpression(string alias_p, LogicalType type, ColumnBinding binding, idx_t depth)
    : Expression(ExpressionType::BOUND_COLUMN_REF, ExpressionClass::BOUND_COLUMN_REF, std::move(type)),
      binding(binding), depth(depth) {
	this->alias = std::move(alias_p);
}

BoundColumnRefExpression::BoundColumnRefExpression(LogicalType type, ColumnBinding binding, idx_t depth)
    : BoundColumnRefExpression(string(), std::move(type), binding, depth) {
}

unique_ptr<Expression> BoundColumnRefExpression::Copy() const {
	return make_uniq<BoundColumnRefExpression>(alias, return_type, binding, depth);
}

hash_t BoundColumnRefExpression::Hash() const {
	auto result = Expression::Hash();
	result = CombineHash(result, goose::Hash<uint64_t>(binding.column_index));
	result = CombineHash(result, goose::Hash<uint64_t>(binding.table_index));
	return CombineHash(result, goose::Hash<uint64_t>(depth));
}

bool BoundColumnRefExpression::Equals(const BaseExpression &other_p) const {
	if (!Expression::Equals(other_p)) {
		return false;
	}
	auto &other = other_p.Cast<BoundColumnRefExpression>();
	return other.binding == binding && other.depth == depth;
}

string BoundColumnRefExpression::GetName() const {
#ifdef DEBUG
	if (DBConfigOptions::debug_print_bindings) {
		return StringUtil::Format("%s (%s)", binding.ToString(), return_type.ToString());
	}
#endif
	return Expression::GetName();
}

string BoundColumnRefExpression::ToString() const {
#ifdef DEBUG
	if (DBConfigOptions::debug_print_bindings) {
		return binding.ToString();
	}
#endif
	if (!alias.empty()) {
		return alias;
	}
	return binding.ToString();
}

} // namespace goose
