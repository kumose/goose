#pragma once

#include <goose/main/appender.h>
#include <goose/main/connection.h>
#include <goose/main/client_context.h>
#include <goose/common/exception.h>
#include <goose/common/types/date.h>
#include <goose/parser/column_definition.h>
#include <goose/parser/parsed_data/create_table_info.h>
#include <goose/parser/constraints/not_null_constraint.h>
#include <goose/catalog/catalog.h>

#include <memory>
#include <cassert>

namespace tpcds {

struct tpcds_table_def {
	const char *name;
	int fl_small;
	int fl_child;
	int first_column;
};

#define CALL_CENTER   0
#define DBGEN_VERSION 24

struct tpcds_append_information {
	tpcds_append_information(goose::ClientContext &context_p, goose::TableCatalogEntry *table)
	    : context(context_p), appender(context_p, *table) {
	}

	goose::ClientContext &context;
	goose::InternalAppender appender;

	tpcds_table_def table_def;

	bool IsNull();
};

} // namespace tpcds
