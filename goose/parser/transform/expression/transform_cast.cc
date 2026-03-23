#include <goose/common/limits.h>
#include <goose/parser/expression/cast_expression.h>
#include <goose/parser/expression/constant_expression.h>
#include <goose/parser/transformer.h>
#include <goose/common/operator/cast_operators.h>
#include <goose/common/types/blob.h>

namespace goose {

unique_ptr<ParsedExpression> Transformer::TransformTypeCast(cantor::PGTypeCast &root) {
	// get the type to cast to
	auto type_name = root.typeName;
	LogicalType target_type = TransformTypeName(*type_name);

	// check for a constant BLOB value, then return ConstantExpression with BLOB
	if (!root.tryCast && target_type == LogicalType::BLOB && root.arg->type == cantor::T_PGAConst) {
		auto c = PGPointerCast<cantor::PGAConst>(root.arg);
		if (c->val.type == cantor::T_PGString) {
			CastParameters parameters;
			if (root.location >= 0) {
				parameters.query_location = NumericCast<idx_t>(root.location);
			}
			auto blob_data = Blob::ToBlob(string(c->val.val.str), parameters);
			auto result = make_uniq<ConstantExpression>(Value::BLOB_RAW(blob_data));
			SetQueryLocation(*result, root.location);
			return std::move(result);
		}
	}
	// transform the expression node
	auto expression = TransformExpression(root.arg);
	bool try_cast = root.tryCast;

	// now create a cast operation
	auto result = make_uniq<CastExpression>(target_type, std::move(expression), try_cast);
	SetQueryLocation(*result, root.location);
	return std::move(result);
}

} // namespace goose
