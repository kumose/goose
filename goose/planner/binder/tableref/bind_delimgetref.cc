#include <goose/parser/tableref/delimgetref.h>
#include <goose/planner/binder.h>
#include <goose/planner/operator/logical_delim_get.h>

namespace goose {

BoundStatement Binder::Bind(DelimGetRef &ref) {
	// Have to add bindings
	idx_t tbl_idx = GenerateTableIndex();
	string internal_name = "__internal_delim_get_ref_" + std::to_string(tbl_idx);

	BoundStatement result;
	result.types = std::move(ref.types);
	result.names = std::move(ref.internal_aliases);
	result.plan = make_uniq<LogicalDelimGet>(tbl_idx, result.types);

	bind_context.AddGenericBinding(tbl_idx, internal_name, result.names, result.types);
	return result;
}

} // namespace goose
