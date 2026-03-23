#include <goose/common/exception.h>
#include <goose/parser/tableref.h>
#include <goose/parser/transformer.h>

namespace goose {

unique_ptr<TableRef> Transformer::TransformTableRefNode(cantor::PGNode &n) {
	auto stack_checker = StackCheck();

	switch (n.type) {
	case cantor::T_PGRangeVar:
		return TransformRangeVar(PGCast<cantor::PGRangeVar>(n));
	case cantor::T_PGJoinExpr:
		return TransformJoin(PGCast<cantor::PGJoinExpr>(n));
	case cantor::T_PGRangeSubselect:
		return TransformRangeSubselect(PGCast<cantor::PGRangeSubselect>(n));
	case cantor::T_PGRangeFunction:
		return TransformRangeFunction(PGCast<cantor::PGRangeFunction>(n));
	case cantor::T_PGPivotExpr:
		return TransformPivot(PGCast<cantor::PGPivotExpr>(n));
	default:
		throw NotImplementedException("From Type %d not supported", n.type);
	}
}

} // namespace goose
