#include <goose/parser/constraint.h>

#include <goose/common/printer.h>

namespace goose {

Constraint::Constraint(ConstraintType type) : type(type) {
}

Constraint::~Constraint() {
}

void Constraint::Print() const {
	Printer::Print(ToString());
}

} // namespace goose
