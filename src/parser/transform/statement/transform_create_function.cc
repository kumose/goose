#include <goose/function/scalar_macro_function.h>
#include <goose/function/table_macro_function.h>
#include <goose/parser/expression/comparison_expression.h>
#include <goose/parser/parsed_data/create_macro_info.h>
#include <goose/parser/statement/create_statement.h>
#include <goose/parser/transformer.h>

namespace goose {

unique_ptr<MacroFunction> Transformer::TransformMacroFunction(cantor::PGFunctionDefinition &def) {
	unique_ptr<MacroFunction> macro_func;
	if (def.function) {
		auto expression = TransformExpression(def.function);
		macro_func = make_uniq<ScalarMacroFunction>(std::move(expression));
	} else if (def.query) {
		auto query_node = TransformSelectNode(*def.query);
		macro_func = make_uniq<TableMacroFunction>(std::move(query_node));
	}

	if (!def.params) {
		return macro_func;
	}

	case_insensitive_set_t parameter_names;
	for (auto node = def.params->head; node != nullptr; node = node->next) {
		auto target = PGPointerCast<cantor::PGNode>(node->data.ptr_value);
		if (target->type != cantor::T_PGFunctionParameter) {
			throw InternalException("TODO");
		}
		auto &param = PGCast<cantor::PGFunctionParameter>(*target);

		// Transform parameter name/type
		if (!parameter_names.insert(param.name).second) {
			throw ParserException("Duplicate parameter '%s' in macro definition", param.name);
		}
		macro_func->parameters.emplace_back(make_uniq<ColumnRefExpression>(param.name));
		macro_func->types.emplace_back(param.typeName ? TransformTypeName(*param.typeName) : LogicalType::UNKNOWN);

		// Transform parameter default value
		if (param.defaultValue) {
			auto default_expr = TransformExpression(PGPointerCast<cantor::PGNode>(param.defaultValue));
			Value default_value;
			if (!ConstructConstantFromExpression(*default_expr, default_value)) {
				throw ParserException("Invalid default value for parameter '%s': %s", param.name,
				                      default_expr->ToString());
			}
			default_expr = make_uniq<ConstantExpression>(std::move(default_value));
			default_expr->SetAlias(param.name);
			macro_func->default_parameters[param.name] = std::move(default_expr);
		} else if (!macro_func->default_parameters.empty()) {
			throw ParserException("Parameter without a default follows parameter with a default");
		}
	}

	return macro_func;
}

unique_ptr<CreateStatement> Transformer::TransformCreateFunction(cantor::PGCreateFunctionStmt &stmt) {
	D_ASSERT(stmt.type == cantor::T_PGCreateFunctionStmt);
	D_ASSERT(stmt.functions);

	auto result = make_uniq<CreateStatement>();
	auto qname = TransformQualifiedName(*stmt.name);

	vector<unique_ptr<MacroFunction>> macros;
	for (auto c = stmt.functions->head; c != nullptr; c = lnext(c)) {
		auto &function_def = *PGPointerCast<cantor::PGFunctionDefinition>(c->data.ptr_value);
		macros.push_back(TransformMacroFunction(function_def));
	}
	PivotEntryCheck("macro");

	auto catalog_type =
	    macros[0]->type == MacroType::SCALAR_MACRO ? CatalogType::MACRO_ENTRY : CatalogType::TABLE_MACRO_ENTRY;
	auto info = make_uniq<CreateMacroInfo>(catalog_type);
	info->catalog = qname.catalog;
	info->schema = qname.schema;
	info->name = qname.name;

	// temporary macro
	switch (stmt.name->relpersistence) {
	case cantor::PG_RELPERSISTENCE_TEMP:
		info->temporary = true;
		break;
	case cantor::PG_RELPERSISTENCE_UNLOGGED:
		throw ParserException("Unlogged flag not supported for macros: '%s'", qname.name);
	case cantor::RELPERSISTENCE_PERMANENT:
		info->temporary = false;
		break;
	default:
		throw ParserException("Unsupported persistence flag for table '%s'", qname.name);
	}

	// what to do on conflict
	info->on_conflict = TransformOnConflict(stmt.onconflict);
	info->macros = std::move(macros);

	result->info = std::move(info);

	return result;
}

} // namespace goose
