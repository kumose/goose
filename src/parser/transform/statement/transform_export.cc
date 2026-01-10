#include <goose/parser/statement/export_statement.h>
#include <goose/parser/transformer.h>

namespace goose {

unique_ptr<ExportStatement> Transformer::TransformExport(cantor::PGExportStmt &stmt) {
	auto info = make_uniq<CopyInfo>();
	info->file_path = stmt.filename;
	info->format = "csv";
	info->is_from = false;
	// handle export options
	TransformCopyOptions(*info, stmt.options);

	auto result = make_uniq<ExportStatement>(std::move(info));
	if (stmt.database) {
		result->database = stmt.database;
	}
	return result;
}

} // namespace goose
