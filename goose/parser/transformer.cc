#include <goose/parser/transformer.h>

#include <goose/parser/expression/list.h>
#include <goose/parser/statement/list.h>
#include <goose/parser/query_node/select_node.h>
#include <goose/parser/parser_options.h>

namespace goose {

Transformer::Transformer(ParserOptions &options)
    : parent(nullptr), options(options), stack_depth(DConstants::INVALID_INDEX) {
}

Transformer::Transformer(Transformer &parent)
    : parent(&parent), options(parent.options), stack_depth(DConstants::INVALID_INDEX) {
}

Transformer::~Transformer() {
}

void Transformer::Clear() {
	ClearParameters();
	pivot_entries.clear();
}

bool Transformer::TransformParseTree(cantor::PGList *tree, vector<unique_ptr<SQLStatement>> &statements) {
	InitializeStackCheck();
	for (auto entry = tree->head; entry != nullptr; entry = entry->next) {
		Clear();
		auto n = PGPointerCast<cantor::PGNode>(entry->data.ptr_value);
		auto stmt = TransformStatement(*n);
		D_ASSERT(stmt);
		if (HasPivotEntries()) {
			stmt = CreatePivotStatement(std::move(stmt));
		}
		statements.push_back(std::move(stmt));
	}
	return true;
}

void Transformer::InitializeStackCheck() {
	stack_depth = 0;
}

StackChecker<Transformer> Transformer::StackCheck(idx_t extra_stack) {
	auto &root = RootTransformer();
	D_ASSERT(root.stack_depth != DConstants::INVALID_INDEX);
	if (root.stack_depth + extra_stack >= options.max_expression_depth) {
		throw ParserException("Max expression depth limit of %lld exceeded. Use \"SET max_expression_depth TO x\" to "
		                      "increase the maximum expression depth.",
		                      options.max_expression_depth);
	}
	return StackChecker<Transformer>(root, extra_stack);
}

unique_ptr<SQLStatement> Transformer::TransformStatement(cantor::PGNode &stmt) {
	auto result = TransformStatementInternal(stmt);
	if (!named_param_map.empty()) {
		// Avoid overriding a previous move with nothing
		result->named_param_map = named_param_map;
	}
	return result;
}

Transformer &Transformer::RootTransformer() {
	reference<Transformer> node = *this;
	while (node.get().parent) {
		node = *node.get().parent;
	}
	return node.get();
}

const Transformer &Transformer::RootTransformer() const {
	reference<const Transformer> node = *this;
	while (node.get().parent) {
		node = *node.get().parent;
	}
	return node.get();
}

idx_t Transformer::ParamCount() const {
	auto &root = RootTransformer();
	return root.prepared_statement_parameter_index;
}

void Transformer::SetParamCount(idx_t new_count) {
	auto &root = RootTransformer();
	root.prepared_statement_parameter_index = new_count;
}

void Transformer::ClearParameters() {
	auto &root = RootTransformer();
	root.prepared_statement_parameter_index = 0;
	root.named_param_map.clear();
}

static void ParamTypeCheck(PreparedParamType last_type, PreparedParamType new_type) {
	// Mixing positional/auto-increment and named parameters is not supported
	if (last_type == PreparedParamType::INVALID) {
		return;
	}
	if (last_type == PreparedParamType::NAMED) {
		if (new_type != PreparedParamType::NAMED) {
			throw NotImplementedException("Mixing named and positional parameters is not supported yet");
		}
	}
	if (last_type != PreparedParamType::NAMED) {
		if (new_type == PreparedParamType::NAMED) {
			throw NotImplementedException("Mixing named and positional parameters is not supported yet");
		}
	}
}

void Transformer::SetParam(const string &identifier, idx_t index, PreparedParamType type) {
	auto &root = RootTransformer();
	ParamTypeCheck(root.last_param_type, type);
	root.last_param_type = type;
	D_ASSERT(!root.named_param_map.count(identifier));
	root.named_param_map[identifier] = index;
}

bool Transformer::GetParam(const string &identifier, idx_t &index, PreparedParamType type) {
	auto &root = RootTransformer();
	ParamTypeCheck(root.last_param_type, type);
	auto entry = root.named_param_map.find(identifier);
	if (entry == root.named_param_map.end()) {
		return false;
	}
	index = entry->second;
	return true;
}

unique_ptr<SQLStatement> Transformer::TransformStatementInternal(cantor::PGNode &stmt) {
	switch (stmt.type) {
	case cantor::T_PGRawStmt: {
		auto &raw_stmt = PGCast<cantor::PGRawStmt>(stmt);
		auto result = TransformStatement(*raw_stmt.stmt);
		if (result) {
			result->stmt_location = NumericCast<idx_t>(raw_stmt.stmt_location);
			result->stmt_length = NumericCast<idx_t>(raw_stmt.stmt_len);
		}
		return result;
	}
	case cantor::T_PGSelectStmt:
		return TransformSelectStmt(PGCast<cantor::PGSelectStmt>(stmt));
	case cantor::T_PGCreateStmt:
		return TransformCreateTable(PGCast<cantor::PGCreateStmt>(stmt));
	case cantor::T_PGCreateSchemaStmt:
		return TransformCreateSchema(PGCast<cantor::PGCreateSchemaStmt>(stmt));
	case cantor::T_PGViewStmt:
		return TransformCreateView(PGCast<cantor::PGViewStmt>(stmt));
	case cantor::T_PGCreateSeqStmt:
		return TransformCreateSequence(PGCast<cantor::PGCreateSeqStmt>(stmt));
	case cantor::T_PGCreateFunctionStmt:
		return TransformCreateFunction(PGCast<cantor::PGCreateFunctionStmt>(stmt));
	case cantor::T_PGDropStmt:
		return TransformDrop(PGCast<cantor::PGDropStmt>(stmt));
	case cantor::T_PGInsertStmt:
		return TransformInsert(PGCast<cantor::PGInsertStmt>(stmt));
	case cantor::T_PGCopyStmt:
		return TransformCopy(PGCast<cantor::PGCopyStmt>(stmt));
	case cantor::T_PGTransactionStmt:
		return TransformTransaction(PGCast<cantor::PGTransactionStmt>(stmt));
	case cantor::T_PGDeleteStmt:
		return TransformDelete(PGCast<cantor::PGDeleteStmt>(stmt));
	case cantor::T_PGUpdateStmt:
		return TransformUpdate(PGCast<cantor::PGUpdateStmt>(stmt));
	case cantor::T_PGUpdateExtensionsStmt:
		return TransformUpdateExtensions(PGCast<cantor::PGUpdateExtensionsStmt>(stmt));
	case cantor::T_PGIndexStmt:
		return TransformCreateIndex(PGCast<cantor::PGIndexStmt>(stmt));
	case cantor::T_PGAlterTableStmt:
		return TransformAlter(PGCast<cantor::PGAlterTableStmt>(stmt));
	case cantor::T_PGAlterDatabaseStmt:
		return TransformAlterDatabase(PGCast<cantor::PGAlterDatabaseStmt>(stmt));
	case cantor::T_PGRenameStmt:
		return TransformRename(PGCast<cantor::PGRenameStmt>(stmt));
	case cantor::T_PGPrepareStmt:
		return TransformPrepare(PGCast<cantor::PGPrepareStmt>(stmt));
	case cantor::T_PGExecuteStmt:
		return TransformExecute(PGCast<cantor::PGExecuteStmt>(stmt));
	case cantor::T_PGDeallocateStmt:
		return TransformDeallocate(PGCast<cantor::PGDeallocateStmt>(stmt));
	case cantor::T_PGCreateTableAsStmt:
		return TransformCreateTableAs(PGCast<cantor::PGCreateTableAsStmt>(stmt));
	case cantor::T_PGPragmaStmt:
		return TransformPragma(PGCast<cantor::PGPragmaStmt>(stmt));
	case cantor::T_PGExportStmt:
		return TransformExport(PGCast<cantor::PGExportStmt>(stmt));
	case cantor::T_PGImportStmt:
		return TransformImport(PGCast<cantor::PGImportStmt>(stmt));
	case cantor::T_PGExplainStmt:
		return TransformExplain(PGCast<cantor::PGExplainStmt>(stmt));
	case cantor::T_PGVacuumStmt:
		return TransformVacuum(PGCast<cantor::PGVacuumStmt>(stmt));
	case cantor::T_PGVariableShowStmt:
		return TransformShowStmt(PGCast<cantor::PGVariableShowStmt>(stmt));
	case cantor::T_PGVariableShowSelectStmt:
		return TransformShowSelectStmt(PGCast<cantor::PGVariableShowSelectStmt>(stmt));
	case cantor::T_PGCallStmt:
		return TransformCall(PGCast<cantor::PGCallStmt>(stmt));
	case cantor::T_PGVariableSetStmt:
		return TransformSet(PGCast<cantor::PGVariableSetStmt>(stmt));
	case cantor::T_PGCheckPointStmt:
		return TransformCheckpoint(PGCast<cantor::PGCheckPointStmt>(stmt));
	case cantor::T_PGLoadStmt:
		return TransformLoad(PGCast<cantor::PGLoadStmt>(stmt));
	case cantor::T_PGCreateTypeStmt:
		return TransformCreateType(PGCast<cantor::PGCreateTypeStmt>(stmt));
	case cantor::T_PGAlterSeqStmt:
		return TransformAlterSequence(PGCast<cantor::PGAlterSeqStmt>(stmt));
	case cantor::T_PGAttachStmt:
		return TransformAttach(PGCast<cantor::PGAttachStmt>(stmt));
	case cantor::T_PGDetachStmt:
		return TransformDetach(PGCast<cantor::PGDetachStmt>(stmt));
	case cantor::T_PGUseStmt:
		return TransformUse(PGCast<cantor::PGUseStmt>(stmt));
	case cantor::T_PGCopyDatabaseStmt:
		return TransformCopyDatabase(PGCast<cantor::PGCopyDatabaseStmt>(stmt));
	case cantor::T_PGCreateSecretStmt:
		return TransformSecret(PGCast<cantor::PGCreateSecretStmt>(stmt));
	case cantor::T_PGDropSecretStmt:
		return TransformDropSecret(PGCast<cantor::PGDropSecretStmt>(stmt));
	case cantor::T_PGCommentOnStmt:
		return TransformCommentOn(PGCast<cantor::PGCommentOnStmt>(stmt));
	case cantor::T_PGMergeIntoStmt:
		return TransformMergeInto(PGCast<cantor::PGMergeIntoStmt>(stmt));
	default:
		throw NotImplementedException(NodetypeToString(stmt.type));
	}
}

void Transformer::SetQueryLocation(ParsedExpression &expr, int query_location) {
	if (query_location < 0) {
		return;
	}
	expr.SetQueryLocation(optional_idx(static_cast<idx_t>(query_location)));
}

void Transformer::SetQueryLocation(TableRef &ref, int query_location) {
	if (query_location < 0) {
		return;
	}
	ref.query_location = optional_idx(static_cast<idx_t>(query_location));
}

} // namespace goose
