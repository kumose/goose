#include "add_numbers.h"

#include "goose_extension.h"

GOOSE_EXTENSION_EXTERN

// Scalar function that adds two numbers together
static void AddNumbersTogether(goose_function_info info, goose_data_chunk input, goose_vector output) {
	// get the total number of rows in this chunk
	idx_t input_size = goose_data_chunk_get_size(input);
	// extract the two input vectors
	goose_vector a = goose_data_chunk_get_vector(input, 0);
	goose_vector b = goose_data_chunk_get_vector(input, 1);
	// get the data pointers for the input vectors (both int64 as specified by the parameter types)
	auto a_data = (int64_t *)goose_vector_get_data(a);
	auto b_data = (int64_t *)goose_vector_get_data(b);
	auto result_data = (int64_t *)goose_vector_get_data(output);
	// get the validity vectors
	auto a_validity = goose_vector_get_validity(a);
	auto b_validity = goose_vector_get_validity(b);
	if (a_validity || b_validity) {
		// if either a_validity or b_validity is defined there might be NULL values
		goose_vector_ensure_validity_writable(output);
		auto result_validity = goose_vector_get_validity(output);
		for (idx_t row = 0; row < input_size; row++) {
			if (goose_validity_row_is_valid(a_validity, row) && goose_validity_row_is_valid(b_validity, row)) {
				// not null - do the addition
				result_data[row] = a_data[row] + b_data[row];
			} else {
				// either a or b is NULL - set the result row to NULL
				goose_validity_set_row_invalid(result_validity, row);
			}
		}
	} else {
		// no NULL values - iterate and do the operation directly
		for (idx_t row = 0; row < input_size; row++) {
			result_data[row] = a_data[row] + b_data[row];
		}
	}
}

// Register the AddNumbersFunction
void RegisterAddNumbersFunction(goose_connection connection) {
	// create a scalar function
	auto function = goose_create_scalar_function();
	goose_scalar_function_set_name(function, "add_numbers_together");

	// add a two bigint parameters
	goose_logical_type type = goose_create_logical_type(GOOSE_TYPE_BIGINT);
	goose_scalar_function_add_parameter(function, type);
	goose_scalar_function_add_parameter(function, type);

	// set the return type to bigint
	goose_scalar_function_set_return_type(function, type);

	goose_destroy_logical_type(&type);

	// set up the function
	goose_scalar_function_set_function(function, AddNumbersTogether);

	// register and cleanup
	goose_register_scalar_function(connection, function);
	goose_destroy_scalar_function(&function);
}
