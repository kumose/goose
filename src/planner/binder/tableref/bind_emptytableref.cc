#include <goose/parser/tableref/emptytableref.h>
#include <goose/planner/binder.h>
#include <goose/planner/operator/logical_dummy_scan.h>

namespace goose {

BoundStatement Binder::Bind(EmptyTableRef &ref) {
	BoundStatement result;
	result.plan = make_uniq<LogicalDummyScan>(GenerateTableIndex());
	return result;
}

} // namespace goose
