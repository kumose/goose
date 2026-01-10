#include <goose/parser/base_expression.h>

#include <goose/main/config.h>
#include <goose/common/printer.h>

namespace goose {

void BaseExpression::Print() const {
	Printer::Print(ToString());
}

string BaseExpression::GetName() const {
#ifdef DEBUG
	if (DBConfigOptions::debug_print_bindings) {
		return ToString();
	}
#endif
	return !alias.empty() ? alias : ToString();
}

bool BaseExpression::Equals(const BaseExpression &other) const {
	if (expression_class != other.expression_class || type != other.type) {
		return false;
	}
	return true;
}

void BaseExpression::Verify() const {
}

} // namespace goose
