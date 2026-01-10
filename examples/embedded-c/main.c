#include <goose/goose-c.h>
#include <stdio.h>

int main() {
	goose_database db = NULL;
	goose_connection con = NULL;
	goose_result result;

	if (goose_open(NULL, &db) == GooseError) {
		fprintf(stderr, "Failed to open database\n");
		goto cleanup;
	}
	if (goose_connect(db, &con) == GooseError) {
		fprintf(stderr, "Failed to open connection\n");
		goto cleanup;
	}
	if (goose_query(con, "CREATE TABLE integers(i INTEGER, j INTEGER);", NULL) == GooseError) {
		fprintf(stderr, "Failed to query database\n");
		goto cleanup;
	}
	if (goose_query(con, "INSERT INTO integers VALUES (3, 4), (5, 6), (7, NULL);", NULL) == GooseError) {
		fprintf(stderr, "Failed to query database\n");
		goto cleanup;
	}
	if (goose_query(con, "SELECT * FROM integers", &result) == GooseError) {
		fprintf(stderr, "Failed to query database\n");
		goto cleanup;
	}
	// print the names of the result
	idx_t row_count = goose_row_count(&result);
	idx_t column_count = goose_column_count(&result);
	for (size_t i = 0; i < column_count; i++) {
		printf("%s ", goose_column_name(&result, i));
	}
	printf("\n");
	// print the data of the result
	for (size_t row_idx = 0; row_idx < row_count; row_idx++) {
		for (size_t col_idx = 0; col_idx < column_count; col_idx++) {
			char *val = goose_value_varchar(&result, col_idx, row_idx);
			printf("%s ", val);
			goose_free(val);
		}
		printf("\n");
	}
	// goose_print_result(result);
cleanup:
	goose_destroy_result(&result);
	goose_disconnect(&con);
	goose_close(&db);
}
