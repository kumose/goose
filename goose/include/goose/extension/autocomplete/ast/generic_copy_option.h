#pragma once

#include <goose/parser/expression/constant_expression.h>

#include <goose/common/string.h>
#include <goose/common/types/value.h>

namespace goose {

struct GenericCopyOption {
	string name;
	vector<Value> children; // Default value
	unique_ptr<ParsedExpression> expression = nullptr;

	GenericCopyOption() {

	};

	GenericCopyOption(const string &name_p, const Value &value) : name(name_p) {
		children.push_back(value);
	}

	GenericCopyOption(const GenericCopyOption &other)
	    : name(other.name), children(other.children),
	      expression(other.expression ? other.expression->Copy() : nullptr) {
	}

	GenericCopyOption &operator=(const GenericCopyOption &other) {
		if (this == &other) {
			return *this;
		}

		name = other.name;
		children = other.children;
		expression = other.expression ? other.expression->Copy() : nullptr;

		return *this;
	}
	GenericCopyOption(GenericCopyOption &&other) noexcept = default;
	GenericCopyOption &operator=(GenericCopyOption &&other) noexcept = default;

	unique_ptr<ParsedExpression> GetFirstChildOrExpression() {
		if (!children.empty()) {
			return make_uniq<ConstantExpression>(children[0]);
		}
		if (!expression) {
			throw InvalidInputException("No expression in GenericCopyOption");
		}
		return expression->Copy();
	}
};

} // namespace goose
