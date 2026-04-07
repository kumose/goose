// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <goose/goose-c.h>

//===--------------------------------------------------------------------===//
// Function pointer struct
//===--------------------------------------------------------------------===//
typedef struct {
	// v1.2.0
	goose_state (*goose_open)(const char *path, goose_database *out_database);
	goose_state (*goose_open_ext)(const char *path, goose_database *out_database, goose_config config,
	                                char **out_error);
	void (*goose_close)(goose_database *database);
	goose_state (*goose_connect)(goose_database database, goose_connection *out_connection);
	void (*goose_interrupt)(goose_connection connection);
	goose_query_progress_type (*goose_query_progress)(goose_connection connection);
	void (*goose_disconnect)(goose_connection *connection);
	const char *(*goose_library_version)();
	goose_state (*goose_create_config)(goose_config *out_config);
	size_t (*goose_config_count)();
	goose_state (*goose_get_config_flag)(size_t index, const char **out_name, const char **out_description);
	goose_state (*goose_set_config)(goose_config config, const char *name, const char *option);
	void (*goose_destroy_config)(goose_config *config);
	goose_state (*goose_query)(goose_connection connection, const char *query, goose_result *out_result);
	void (*goose_destroy_result)(goose_result *result);
	const char *(*goose_column_name)(goose_result *result, idx_t col);
	goose_type (*goose_column_type)(goose_result *result, idx_t col);
	goose_statement_type (*goose_result_statement_type)(goose_result result);
	goose_logical_type (*goose_column_logical_type)(goose_result *result, idx_t col);
	idx_t (*goose_column_count)(goose_result *result);
	idx_t (*goose_rows_changed)(goose_result *result);
	const char *(*goose_result_error)(goose_result *result);
	goose_error_type (*goose_result_error_type)(goose_result *result);
	goose_result_type (*goose_result_return_type)(goose_result result);
	void *(*goose_malloc)(size_t size);
	void (*goose_free)(void *ptr);
	idx_t (*goose_vector_size)();
	bool (*goose_string_is_inlined)(goose_string_t string);
	uint32_t (*goose_string_t_length)(goose_string_t string);
	const char *(*goose_string_t_data)(goose_string_t *string);
	goose_date_struct (*goose_from_date)(goose_date date);
	goose_date (*goose_to_date)(goose_date_struct date);
	bool (*goose_is_finite_date)(goose_date date);
	goose_time_struct (*goose_from_time)(goose_time time);
	goose_time_tz (*goose_create_time_tz)(int64_t micros, int32_t offset);
	goose_time_tz_struct (*goose_from_time_tz)(goose_time_tz micros);
	goose_time (*goose_to_time)(goose_time_struct time);
	goose_timestamp_struct (*goose_from_timestamp)(goose_timestamp ts);
	goose_timestamp (*goose_to_timestamp)(goose_timestamp_struct ts);
	bool (*goose_is_finite_timestamp)(goose_timestamp ts);
	double (*goose_hugeint_to_double)(goose_hugeint val);
	goose_hugeint (*goose_double_to_hugeint)(double val);
	double (*goose_uhugeint_to_double)(goose_uhugeint val);
	goose_uhugeint (*goose_double_to_uhugeint)(double val);
	goose_decimal (*goose_double_to_decimal)(double val, uint8_t width, uint8_t scale);
	double (*goose_decimal_to_double)(goose_decimal val);
	goose_state (*goose_prepare)(goose_connection connection, const char *query,
	                               goose_prepared_statement *out_prepared_statement);
	void (*goose_destroy_prepare)(goose_prepared_statement *prepared_statement);
	const char *(*goose_prepare_error)(goose_prepared_statement prepared_statement);
	idx_t (*goose_nparams)(goose_prepared_statement prepared_statement);
	const char *(*goose_parameter_name)(goose_prepared_statement prepared_statement, idx_t index);
	goose_type (*goose_param_type)(goose_prepared_statement prepared_statement, idx_t param_idx);
	goose_logical_type (*goose_param_logical_type)(goose_prepared_statement prepared_statement, idx_t param_idx);
	goose_state (*goose_clear_bindings)(goose_prepared_statement prepared_statement);
	goose_statement_type (*goose_prepared_statement_type)(goose_prepared_statement statement);
	goose_state (*goose_bind_value)(goose_prepared_statement prepared_statement, idx_t param_idx, goose_value val);
	goose_state (*goose_bind_parameter_index)(goose_prepared_statement prepared_statement, idx_t *param_idx_out,
	                                            const char *name);
	goose_state (*goose_bind_boolean)(goose_prepared_statement prepared_statement, idx_t param_idx, bool val);
	goose_state (*goose_bind_int8)(goose_prepared_statement prepared_statement, idx_t param_idx, int8_t val);
	goose_state (*goose_bind_int16)(goose_prepared_statement prepared_statement, idx_t param_idx, int16_t val);
	goose_state (*goose_bind_int32)(goose_prepared_statement prepared_statement, idx_t param_idx, int32_t val);
	goose_state (*goose_bind_int64)(goose_prepared_statement prepared_statement, idx_t param_idx, int64_t val);
	goose_state (*goose_bind_hugeint)(goose_prepared_statement prepared_statement, idx_t param_idx,
	                                    goose_hugeint val);
	goose_state (*goose_bind_uhugeint)(goose_prepared_statement prepared_statement, idx_t param_idx,
	                                     goose_uhugeint val);
	goose_state (*goose_bind_decimal)(goose_prepared_statement prepared_statement, idx_t param_idx,
	                                    goose_decimal val);
	goose_state (*goose_bind_uint8)(goose_prepared_statement prepared_statement, idx_t param_idx, uint8_t val);
	goose_state (*goose_bind_uint16)(goose_prepared_statement prepared_statement, idx_t param_idx, uint16_t val);
	goose_state (*goose_bind_uint32)(goose_prepared_statement prepared_statement, idx_t param_idx, uint32_t val);
	goose_state (*goose_bind_uint64)(goose_prepared_statement prepared_statement, idx_t param_idx, uint64_t val);
	goose_state (*goose_bind_float)(goose_prepared_statement prepared_statement, idx_t param_idx, float val);
	goose_state (*goose_bind_double)(goose_prepared_statement prepared_statement, idx_t param_idx, double val);
	goose_state (*goose_bind_date)(goose_prepared_statement prepared_statement, idx_t param_idx, goose_date val);
	goose_state (*goose_bind_time)(goose_prepared_statement prepared_statement, idx_t param_idx, goose_time val);
	goose_state (*goose_bind_timestamp)(goose_prepared_statement prepared_statement, idx_t param_idx,
	                                      goose_timestamp val);
	goose_state (*goose_bind_timestamp_tz)(goose_prepared_statement prepared_statement, idx_t param_idx,
	                                         goose_timestamp val);
	goose_state (*goose_bind_interval)(goose_prepared_statement prepared_statement, idx_t param_idx,
	                                     goose_interval val);
	goose_state (*goose_bind_varchar)(goose_prepared_statement prepared_statement, idx_t param_idx, const char *val);
	goose_state (*goose_bind_varchar_length)(goose_prepared_statement prepared_statement, idx_t param_idx,
	                                           const char *val, idx_t length);
	goose_state (*goose_bind_blob)(goose_prepared_statement prepared_statement, idx_t param_idx, const void *data,
	                                 idx_t length);
	goose_state (*goose_bind_null)(goose_prepared_statement prepared_statement, idx_t param_idx);
	goose_state (*goose_execute_prepared)(goose_prepared_statement prepared_statement, goose_result *out_result);
	idx_t (*goose_extract_statements)(goose_connection connection, const char *query,
	                                   goose_extracted_statements *out_extracted_statements);
	goose_state (*goose_prepare_extracted_statement)(goose_connection connection,
	                                                   goose_extracted_statements extracted_statements, idx_t index,
	                                                   goose_prepared_statement *out_prepared_statement);
	const char *(*goose_extract_statements_error)(goose_extracted_statements extracted_statements);
	void (*goose_destroy_extracted)(goose_extracted_statements *extracted_statements);
	goose_state (*goose_pending_prepared)(goose_prepared_statement prepared_statement,
	                                        goose_pending_result *out_result);
	void (*goose_destroy_pending)(goose_pending_result *pending_result);
	const char *(*goose_pending_error)(goose_pending_result pending_result);
	goose_pending_state (*goose_pending_execute_task)(goose_pending_result pending_result);
	goose_pending_state (*goose_pending_execute_check_state)(goose_pending_result pending_result);
	goose_state (*goose_execute_pending)(goose_pending_result pending_result, goose_result *out_result);
	bool (*goose_pending_execution_is_finished)(goose_pending_state pending_state);
	void (*goose_destroy_value)(goose_value *value);
	goose_value (*goose_create_varchar)(const char *text);
	goose_value (*goose_create_varchar_length)(const char *text, idx_t length);
	goose_value (*goose_create_bool)(bool input);
	goose_value (*goose_create_int8)(int8_t input);
	goose_value (*goose_create_uint8)(uint8_t input);
	goose_value (*goose_create_int16)(int16_t input);
	goose_value (*goose_create_uint16)(uint16_t input);
	goose_value (*goose_create_int32)(int32_t input);
	goose_value (*goose_create_uint32)(uint32_t input);
	goose_value (*goose_create_uint64)(uint64_t input);
	goose_value (*goose_create_int64)(int64_t val);
	goose_value (*goose_create_hugeint)(goose_hugeint input);
	goose_value (*goose_create_uhugeint)(goose_uhugeint input);
	goose_value (*goose_create_float)(float input);
	goose_value (*goose_create_double)(double input);
	goose_value (*goose_create_date)(goose_date input);
	goose_value (*goose_create_time)(goose_time input);
	goose_value (*goose_create_time_tz_value)(goose_time_tz value);
	goose_value (*goose_create_timestamp)(goose_timestamp input);
	goose_value (*goose_create_interval)(goose_interval input);
	goose_value (*goose_create_blob)(const uint8_t *data, idx_t length);
	goose_value (*goose_create_bignum)(goose_bignum input);
	goose_value (*goose_create_decimal)(goose_decimal input);
	goose_value (*goose_create_bit)(goose_bit input);
	goose_value (*goose_create_uuid)(goose_uhugeint input);
	bool (*goose_get_bool)(goose_value val);
	int8_t (*goose_get_int8)(goose_value val);
	uint8_t (*goose_get_uint8)(goose_value val);
	int16_t (*goose_get_int16)(goose_value val);
	uint16_t (*goose_get_uint16)(goose_value val);
	int32_t (*goose_get_int32)(goose_value val);
	uint32_t (*goose_get_uint32)(goose_value val);
	int64_t (*goose_get_int64)(goose_value val);
	uint64_t (*goose_get_uint64)(goose_value val);
	goose_hugeint (*goose_get_hugeint)(goose_value val);
	goose_uhugeint (*goose_get_uhugeint)(goose_value val);
	float (*goose_get_float)(goose_value val);
	double (*goose_get_double)(goose_value val);
	goose_date (*goose_get_date)(goose_value val);
	goose_time (*goose_get_time)(goose_value val);
	goose_time_tz (*goose_get_time_tz)(goose_value val);
	goose_timestamp (*goose_get_timestamp)(goose_value val);
	goose_interval (*goose_get_interval)(goose_value val);
	goose_logical_type (*goose_get_value_type)(goose_value val);
	goose_blob (*goose_get_blob)(goose_value val);
	goose_bignum (*goose_get_bignum)(goose_value val);
	goose_decimal (*goose_get_decimal)(goose_value val);
	goose_bit (*goose_get_bit)(goose_value val);
	goose_uhugeint (*goose_get_uuid)(goose_value val);
	char *(*goose_get_varchar)(goose_value value);
	goose_value (*goose_create_struct_value)(goose_logical_type type, goose_value *values);
	goose_value (*goose_create_list_value)(goose_logical_type type, goose_value *values, idx_t value_count);
	goose_value (*goose_create_array_value)(goose_logical_type type, goose_value *values, idx_t value_count);
	idx_t (*goose_get_map_size)(goose_value value);
	goose_value (*goose_get_map_key)(goose_value value, idx_t index);
	goose_value (*goose_get_map_value)(goose_value value, idx_t index);
	bool (*goose_is_null_value)(goose_value value);
	goose_value (*goose_create_null_value)();
	idx_t (*goose_get_list_size)(goose_value value);
	goose_value (*goose_get_list_child)(goose_value value, idx_t index);
	goose_value (*goose_create_enum_value)(goose_logical_type type, uint64_t value);
	uint64_t (*goose_get_enum_value)(goose_value value);
	goose_value (*goose_get_struct_child)(goose_value value, idx_t index);
	goose_logical_type (*goose_create_logical_type)(goose_type type);
	char *(*goose_logical_type_get_alias)(goose_logical_type type);
	void (*goose_logical_type_set_alias)(goose_logical_type type, const char *alias);
	goose_logical_type (*goose_create_list_type)(goose_logical_type type);
	goose_logical_type (*goose_create_array_type)(goose_logical_type type, idx_t array_size);
	goose_logical_type (*goose_create_map_type)(goose_logical_type key_type, goose_logical_type value_type);
	goose_logical_type (*goose_create_union_type)(goose_logical_type *member_types, const char **member_names,
	                                                idx_t member_count);
	goose_logical_type (*goose_create_struct_type)(goose_logical_type *member_types, const char **member_names,
	                                                 idx_t member_count);
	goose_logical_type (*goose_create_enum_type)(const char **member_names, idx_t member_count);
	goose_logical_type (*goose_create_decimal_type)(uint8_t width, uint8_t scale);
	goose_type (*goose_get_type_id)(goose_logical_type type);
	uint8_t (*goose_decimal_width)(goose_logical_type type);
	uint8_t (*goose_decimal_scale)(goose_logical_type type);
	goose_type (*goose_decimal_internal_type)(goose_logical_type type);
	goose_type (*goose_enum_internal_type)(goose_logical_type type);
	uint32_t (*goose_enum_dictionary_size)(goose_logical_type type);
	char *(*goose_enum_dictionary_value)(goose_logical_type type, idx_t index);
	goose_logical_type (*goose_list_type_child_type)(goose_logical_type type);
	goose_logical_type (*goose_array_type_child_type)(goose_logical_type type);
	idx_t (*goose_array_type_array_size)(goose_logical_type type);
	goose_logical_type (*goose_map_type_key_type)(goose_logical_type type);
	goose_logical_type (*goose_map_type_value_type)(goose_logical_type type);
	idx_t (*goose_struct_type_child_count)(goose_logical_type type);
	char *(*goose_struct_type_child_name)(goose_logical_type type, idx_t index);
	goose_logical_type (*goose_struct_type_child_type)(goose_logical_type type, idx_t index);
	idx_t (*goose_union_type_member_count)(goose_logical_type type);
	char *(*goose_union_type_member_name)(goose_logical_type type, idx_t index);
	goose_logical_type (*goose_union_type_member_type)(goose_logical_type type, idx_t index);
	void (*goose_destroy_logical_type)(goose_logical_type *type);
	goose_state (*goose_register_logical_type)(goose_connection con, goose_logical_type type,
	                                             goose_create_type_info info);
	goose_data_chunk (*goose_create_data_chunk)(goose_logical_type *types, idx_t column_count);
	void (*goose_destroy_data_chunk)(goose_data_chunk *chunk);
	void (*goose_data_chunk_reset)(goose_data_chunk chunk);
	idx_t (*goose_data_chunk_get_column_count)(goose_data_chunk chunk);
	goose_vector (*goose_data_chunk_get_vector)(goose_data_chunk chunk, idx_t col_idx);
	idx_t (*goose_data_chunk_get_size)(goose_data_chunk chunk);
	void (*goose_data_chunk_set_size)(goose_data_chunk chunk, idx_t size);
	goose_logical_type (*goose_vector_get_column_type)(goose_vector vector);
	void *(*goose_vector_get_data)(goose_vector vector);
	uint64_t *(*goose_vector_get_validity)(goose_vector vector);
	void (*goose_vector_ensure_validity_writable)(goose_vector vector);
	void (*goose_vector_assign_string_element)(goose_vector vector, idx_t index, const char *str);
	void (*goose_vector_assign_string_element_len)(goose_vector vector, idx_t index, const char *str, idx_t str_len);
	goose_vector (*goose_list_vector_get_child)(goose_vector vector);
	idx_t (*goose_list_vector_get_size)(goose_vector vector);
	goose_state (*goose_list_vector_set_size)(goose_vector vector, idx_t size);
	goose_state (*goose_list_vector_reserve)(goose_vector vector, idx_t required_capacity);
	goose_vector (*goose_struct_vector_get_child)(goose_vector vector, idx_t index);
	goose_vector (*goose_array_vector_get_child)(goose_vector vector);
	bool (*goose_validity_row_is_valid)(uint64_t *validity, idx_t row);
	void (*goose_validity_set_row_validity)(uint64_t *validity, idx_t row, bool valid);
	void (*goose_validity_set_row_invalid)(uint64_t *validity, idx_t row);
	void (*goose_validity_set_row_valid)(uint64_t *validity, idx_t row);
	goose_scalar_function (*goose_create_scalar_function)();
	void (*goose_destroy_scalar_function)(goose_scalar_function *scalar_function);
	void (*goose_scalar_function_set_name)(goose_scalar_function scalar_function, const char *name);
	void (*goose_scalar_function_set_varargs)(goose_scalar_function scalar_function, goose_logical_type type);
	void (*goose_scalar_function_set_special_handling)(goose_scalar_function scalar_function);
	void (*goose_scalar_function_set_volatile)(goose_scalar_function scalar_function);
	void (*goose_scalar_function_add_parameter)(goose_scalar_function scalar_function, goose_logical_type type);
	void (*goose_scalar_function_set_return_type)(goose_scalar_function scalar_function, goose_logical_type type);
	void (*goose_scalar_function_set_extra_info)(goose_scalar_function scalar_function, void *extra_info,
	                                              goose_delete_callback_t destroy);
	void (*goose_scalar_function_set_function)(goose_scalar_function scalar_function,
	                                            goose_scalar_function_t function);
	goose_state (*goose_register_scalar_function)(goose_connection con, goose_scalar_function scalar_function);
	void *(*goose_scalar_function_get_extra_info)(goose_function_info info);
	void (*goose_scalar_function_set_error)(goose_function_info info, const char *error);
	goose_scalar_function_set (*goose_create_scalar_function_set)(const char *name);
	void (*goose_destroy_scalar_function_set)(goose_scalar_function_set *scalar_function_set);
	goose_state (*goose_add_scalar_function_to_set)(goose_scalar_function_set set, goose_scalar_function function);
	goose_state (*goose_register_scalar_function_set)(goose_connection con, goose_scalar_function_set set);
	goose_aggregate_function (*goose_create_aggregate_function)();
	void (*goose_destroy_aggregate_function)(goose_aggregate_function *aggregate_function);
	void (*goose_aggregate_function_set_name)(goose_aggregate_function aggregate_function, const char *name);
	void (*goose_aggregate_function_add_parameter)(goose_aggregate_function aggregate_function,
	                                                goose_logical_type type);
	void (*goose_aggregate_function_set_return_type)(goose_aggregate_function aggregate_function,
	                                                  goose_logical_type type);
	void (*goose_aggregate_function_set_functions)(goose_aggregate_function aggregate_function,
	                                                goose_aggregate_state_size state_size,
	                                                goose_aggregate_init_t state_init,
	                                                goose_aggregate_update_t update,
	                                                goose_aggregate_combine_t combine,
	                                                goose_aggregate_finalize_t finalize);
	void (*goose_aggregate_function_set_destructor)(goose_aggregate_function aggregate_function,
	                                                 goose_aggregate_destroy_t destroy);
	goose_state (*goose_register_aggregate_function)(goose_connection con,
	                                                   goose_aggregate_function aggregate_function);
	void (*goose_aggregate_function_set_special_handling)(goose_aggregate_function aggregate_function);
	void (*goose_aggregate_function_set_extra_info)(goose_aggregate_function aggregate_function, void *extra_info,
	                                                 goose_delete_callback_t destroy);
	void *(*goose_aggregate_function_get_extra_info)(goose_function_info info);
	void (*goose_aggregate_function_set_error)(goose_function_info info, const char *error);
	goose_aggregate_function_set (*goose_create_aggregate_function_set)(const char *name);
	void (*goose_destroy_aggregate_function_set)(goose_aggregate_function_set *aggregate_function_set);
	goose_state (*goose_add_aggregate_function_to_set)(goose_aggregate_function_set set,
	                                                     goose_aggregate_function function);
	goose_state (*goose_register_aggregate_function_set)(goose_connection con, goose_aggregate_function_set set);
	goose_table_function (*goose_create_table_function)();
	void (*goose_destroy_table_function)(goose_table_function *table_function);
	void (*goose_table_function_set_name)(goose_table_function table_function, const char *name);
	void (*goose_table_function_add_parameter)(goose_table_function table_function, goose_logical_type type);
	void (*goose_table_function_add_named_parameter)(goose_table_function table_function, const char *name,
	                                                  goose_logical_type type);
	void (*goose_table_function_set_extra_info)(goose_table_function table_function, void *extra_info,
	                                             goose_delete_callback_t destroy);
	void (*goose_table_function_set_bind)(goose_table_function table_function, goose_table_function_bind_t bind);
	void (*goose_table_function_set_init)(goose_table_function table_function, goose_table_function_init_t init);
	void (*goose_table_function_set_local_init)(goose_table_function table_function,
	                                             goose_table_function_init_t init);
	void (*goose_table_function_set_function)(goose_table_function table_function, goose_table_function_t function);
	void (*goose_table_function_supports_projection_pushdown)(goose_table_function table_function, bool pushdown);
	goose_state (*goose_register_table_function)(goose_connection con, goose_table_function function);
	void *(*goose_bind_get_extra_info)(goose_bind_info info);
	void (*goose_bind_add_result_column)(goose_bind_info info, const char *name, goose_logical_type type);
	idx_t (*goose_bind_get_parameter_count)(goose_bind_info info);
	goose_value (*goose_bind_get_parameter)(goose_bind_info info, idx_t index);
	goose_value (*goose_bind_get_named_parameter)(goose_bind_info info, const char *name);
	void (*goose_bind_set_bind_data)(goose_bind_info info, void *bind_data, goose_delete_callback_t destroy);
	void (*goose_bind_set_cardinality)(goose_bind_info info, idx_t cardinality, bool is_exact);
	void (*goose_bind_set_error)(goose_bind_info info, const char *error);
	void *(*goose_init_get_extra_info)(goose_init_info info);
	void *(*goose_init_get_bind_data)(goose_init_info info);
	void (*goose_init_set_init_data)(goose_init_info info, void *init_data, goose_delete_callback_t destroy);
	idx_t (*goose_init_get_column_count)(goose_init_info info);
	idx_t (*goose_init_get_column_index)(goose_init_info info, idx_t column_index);
	void (*goose_init_set_max_threads)(goose_init_info info, idx_t max_threads);
	void (*goose_init_set_error)(goose_init_info info, const char *error);
	void *(*goose_function_get_extra_info)(goose_function_info info);
	void *(*goose_function_get_bind_data)(goose_function_info info);
	void *(*goose_function_get_init_data)(goose_function_info info);
	void *(*goose_function_get_local_init_data)(goose_function_info info);
	void (*goose_function_set_error)(goose_function_info info, const char *error);
	void (*goose_add_replacement_scan)(goose_database db, goose_replacement_callback_t replacement, void *extra_data,
	                                    goose_delete_callback_t delete_callback);
	void (*goose_replacement_scan_set_function_name)(goose_replacement_scan_info info, const char *function_name);
	void (*goose_replacement_scan_add_parameter)(goose_replacement_scan_info info, goose_value parameter);
	void (*goose_replacement_scan_set_error)(goose_replacement_scan_info info, const char *error);
	goose_value (*goose_profiling_info_get_metrics)(goose_profiling_info info);
	idx_t (*goose_profiling_info_get_child_count)(goose_profiling_info info);
	goose_profiling_info (*goose_profiling_info_get_child)(goose_profiling_info info, idx_t index);
	goose_state (*goose_appender_create)(goose_connection connection, const char *schema, const char *table,
	                                       goose_appender *out_appender);
	goose_state (*goose_appender_create_ext)(goose_connection connection, const char *catalog, const char *schema,
	                                           const char *table, goose_appender *out_appender);
	idx_t (*goose_appender_column_count)(goose_appender appender);
	goose_logical_type (*goose_appender_column_type)(goose_appender appender, idx_t col_idx);
	const char *(*goose_appender_error)(goose_appender appender);
	goose_state (*goose_appender_flush)(goose_appender appender);
	goose_state (*goose_appender_close)(goose_appender appender);
	goose_state (*goose_appender_destroy)(goose_appender *appender);
	goose_state (*goose_appender_add_column)(goose_appender appender, const char *name);
	goose_state (*goose_appender_clear_columns)(goose_appender appender);
	goose_state (*goose_append_data_chunk)(goose_appender appender, goose_data_chunk chunk);
	goose_state (*goose_table_description_create)(goose_connection connection, const char *schema, const char *table,
	                                                goose_table_description *out);
	goose_state (*goose_table_description_create_ext)(goose_connection connection, const char *catalog,
	                                                    const char *schema, const char *table,
	                                                    goose_table_description *out);
	void (*goose_table_description_destroy)(goose_table_description *table_description);
	const char *(*goose_table_description_error)(goose_table_description table_description);
	goose_state (*goose_column_has_default)(goose_table_description table_description, idx_t index, bool *out);
	char *(*goose_table_description_get_column_name)(goose_table_description table_description, idx_t index);
	void (*goose_execute_tasks)(goose_database database, idx_t max_tasks);
	goose_task_state (*goose_create_task_state)(goose_database database);
	void (*goose_execute_tasks_state)(goose_task_state state);
	idx_t (*goose_execute_n_tasks_state)(goose_task_state state, idx_t max_tasks);
	void (*goose_finish_execution)(goose_task_state state);
	bool (*goose_task_state_is_finished)(goose_task_state state);
	void (*goose_destroy_task_state)(goose_task_state state);
	bool (*goose_execution_is_finished)(goose_connection con);
	goose_data_chunk (*goose_fetch_chunk)(goose_result result);
	goose_cast_function (*goose_create_cast_function)();
	void (*goose_cast_function_set_source_type)(goose_cast_function cast_function, goose_logical_type source_type);
	void (*goose_cast_function_set_target_type)(goose_cast_function cast_function, goose_logical_type target_type);
	void (*goose_cast_function_set_implicit_cast_cost)(goose_cast_function cast_function, int64_t cost);
	void (*goose_cast_function_set_function)(goose_cast_function cast_function, goose_cast_function_t function);
	void (*goose_cast_function_set_extra_info)(goose_cast_function cast_function, void *extra_info,
	                                            goose_delete_callback_t destroy);
	void *(*goose_cast_function_get_extra_info)(goose_function_info info);
	goose_cast_mode (*goose_cast_function_get_cast_mode)(goose_function_info info);
	void (*goose_cast_function_set_error)(goose_function_info info, const char *error);
	void (*goose_cast_function_set_row_error)(goose_function_info info, const char *error, idx_t row,
	                                           goose_vector output);
	goose_state (*goose_register_cast_function)(goose_connection con, goose_cast_function cast_function);
	void (*goose_destroy_cast_function)(goose_cast_function *cast_function);
	bool (*goose_is_finite_timestamp_s)(goose_timestamp_s ts);
	bool (*goose_is_finite_timestamp_ms)(goose_timestamp_ms ts);
	bool (*goose_is_finite_timestamp_ns)(goose_timestamp_ns ts);
	goose_value (*goose_create_timestamp_tz)(goose_timestamp input);
	goose_value (*goose_create_timestamp_s)(goose_timestamp_s input);
	goose_value (*goose_create_timestamp_ms)(goose_timestamp_ms input);
	goose_value (*goose_create_timestamp_ns)(goose_timestamp_ns input);
	goose_timestamp (*goose_get_timestamp_tz)(goose_value val);
	goose_timestamp_s (*goose_get_timestamp_s)(goose_value val);
	goose_timestamp_ms (*goose_get_timestamp_ms)(goose_value val);
	goose_timestamp_ns (*goose_get_timestamp_ns)(goose_value val);
	goose_state (*goose_append_value)(goose_appender appender, goose_value value);
	goose_profiling_info (*goose_get_profiling_info)(goose_connection connection);
	goose_value (*goose_profiling_info_get_value)(goose_profiling_info info, const char *key);
	goose_state (*goose_appender_begin_row)(goose_appender appender);
	goose_state (*goose_appender_end_row)(goose_appender appender);
	goose_state (*goose_append_default)(goose_appender appender);
	goose_state (*goose_append_bool)(goose_appender appender, bool value);
	goose_state (*goose_append_int8)(goose_appender appender, int8_t value);
	goose_state (*goose_append_int16)(goose_appender appender, int16_t value);
	goose_state (*goose_append_int32)(goose_appender appender, int32_t value);
	goose_state (*goose_append_int64)(goose_appender appender, int64_t value);
	goose_state (*goose_append_hugeint)(goose_appender appender, goose_hugeint value);
	goose_state (*goose_append_uint8)(goose_appender appender, uint8_t value);
	goose_state (*goose_append_uint16)(goose_appender appender, uint16_t value);
	goose_state (*goose_append_uint32)(goose_appender appender, uint32_t value);
	goose_state (*goose_append_uint64)(goose_appender appender, uint64_t value);
	goose_state (*goose_append_uhugeint)(goose_appender appender, goose_uhugeint value);
	goose_state (*goose_append_float)(goose_appender appender, float value);
	goose_state (*goose_append_double)(goose_appender appender, double value);
	goose_state (*goose_append_date)(goose_appender appender, goose_date value);
	goose_state (*goose_append_time)(goose_appender appender, goose_time value);
	goose_state (*goose_append_timestamp)(goose_appender appender, goose_timestamp value);
	goose_state (*goose_append_interval)(goose_appender appender, goose_interval value);
	goose_state (*goose_append_varchar)(goose_appender appender, const char *val);
	goose_state (*goose_append_varchar_length)(goose_appender appender, const char *val, idx_t length);
	goose_state (*goose_append_blob)(goose_appender appender, const void *data, idx_t length);
	goose_state (*goose_append_null)(goose_appender appender);
	// These functions have been deprecated and may be removed in future versions of Goose

	idx_t (*goose_row_count)(goose_result *result);
	void *(*goose_column_data)(goose_result *result, idx_t col);
	bool *(*goose_nullmask_data)(goose_result *result, idx_t col);
	goose_data_chunk (*goose_result_get_chunk)(goose_result result, idx_t chunk_index);
	bool (*goose_result_is_streaming)(goose_result result);
	idx_t (*goose_result_chunk_count)(goose_result result);
	bool (*goose_value_boolean)(goose_result *result, idx_t col, idx_t row);
	int8_t (*goose_value_int8)(goose_result *result, idx_t col, idx_t row);
	int16_t (*goose_value_int16)(goose_result *result, idx_t col, idx_t row);
	int32_t (*goose_value_int32)(goose_result *result, idx_t col, idx_t row);
	int64_t (*goose_value_int64)(goose_result *result, idx_t col, idx_t row);
	goose_hugeint (*goose_value_hugeint)(goose_result *result, idx_t col, idx_t row);
	goose_uhugeint (*goose_value_uhugeint)(goose_result *result, idx_t col, idx_t row);
	goose_decimal (*goose_value_decimal)(goose_result *result, idx_t col, idx_t row);
	uint8_t (*goose_value_uint8)(goose_result *result, idx_t col, idx_t row);
	uint16_t (*goose_value_uint16)(goose_result *result, idx_t col, idx_t row);
	uint32_t (*goose_value_uint32)(goose_result *result, idx_t col, idx_t row);
	uint64_t (*goose_value_uint64)(goose_result *result, idx_t col, idx_t row);
	float (*goose_value_float)(goose_result *result, idx_t col, idx_t row);
	double (*goose_value_double)(goose_result *result, idx_t col, idx_t row);
	goose_date (*goose_value_date)(goose_result *result, idx_t col, idx_t row);
	goose_time (*goose_value_time)(goose_result *result, idx_t col, idx_t row);
	goose_timestamp (*goose_value_timestamp)(goose_result *result, idx_t col, idx_t row);
	goose_interval (*goose_value_interval)(goose_result *result, idx_t col, idx_t row);
	char *(*goose_value_varchar)(goose_result *result, idx_t col, idx_t row);
	goose_string (*goose_value_string)(goose_result *result, idx_t col, idx_t row);
	char *(*goose_value_varchar_internal)(goose_result *result, idx_t col, idx_t row);
	goose_string (*goose_value_string_internal)(goose_result *result, idx_t col, idx_t row);
	goose_blob (*goose_value_blob)(goose_result *result, idx_t col, idx_t row);
	bool (*goose_value_is_null)(goose_result *result, idx_t col, idx_t row);
	goose_state (*goose_execute_prepared_streaming)(goose_prepared_statement prepared_statement,
	                                                  goose_result *out_result);
	goose_state (*goose_pending_prepared_streaming)(goose_prepared_statement prepared_statement,
	                                                  goose_pending_result *out_result);
	goose_state (*goose_query_arrow)(goose_connection connection, const char *query, goose_arrow *out_result);
	goose_state (*goose_query_arrow_schema)(goose_arrow result, goose_arrow_schema *out_schema);
	goose_state (*goose_prepared_arrow_schema)(goose_prepared_statement prepared, goose_arrow_schema *out_schema);
	void (*goose_result_arrow_array)(goose_result result, goose_data_chunk chunk, goose_arrow_array *out_array);
	goose_state (*goose_query_arrow_array)(goose_arrow result, goose_arrow_array *out_array);
	idx_t (*goose_arrow_column_count)(goose_arrow result);
	idx_t (*goose_arrow_row_count)(goose_arrow result);
	idx_t (*goose_arrow_rows_changed)(goose_arrow result);
	const char *(*goose_query_arrow_error)(goose_arrow result);
	void (*goose_destroy_arrow)(goose_arrow *result);
	void (*goose_destroy_arrow_stream)(goose_arrow_stream *stream_p);
	goose_state (*goose_execute_prepared_arrow)(goose_prepared_statement prepared_statement,
	                                              goose_arrow *out_result);
	goose_state (*goose_arrow_scan)(goose_connection connection, const char *table_name, goose_arrow_stream arrow);
	goose_state (*goose_arrow_array_scan)(goose_connection connection, const char *table_name,
	                                        goose_arrow_schema arrow_schema, goose_arrow_array arrow_array,
	                                        goose_arrow_stream *out_stream);
	goose_data_chunk (*goose_stream_fetch_chunk)(goose_result result);
	// Exposing the instance cache

	goose_instance_cache (*goose_create_instance_cache)();
	goose_state (*goose_get_or_create_from_cache)(goose_instance_cache instance_cache, const char *path,
	                                                goose_database *out_database, goose_config config,
	                                                char **out_error);
	void (*goose_destroy_instance_cache)(goose_instance_cache *instance_cache);
	// New append functions that are added

	goose_state (*goose_append_default_to_chunk)(goose_appender appender, goose_data_chunk chunk, idx_t col,
	                                               idx_t row);
	goose_error_data (*goose_appender_error_data)(goose_appender appender);
	goose_state (*goose_appender_create_query)(goose_connection connection, const char *query, idx_t column_count,
	                                             goose_logical_type *types, const char *table_name,
	                                             const char **column_names, goose_appender *out_appender);
	goose_state (*goose_appender_clear)(goose_appender appender);
	// New arrow interface functions

	goose_error_data (*goose_to_arrow_schema)(goose_arrow_options arrow_options, goose_logical_type *types,
	                                            const char **names, idx_t column_count, struct ArrowSchema *out_schema);
	goose_error_data (*goose_data_chunk_to_arrow)(goose_arrow_options arrow_options, goose_data_chunk chunk,
	                                                struct ArrowArray *out_arrow_array);
	goose_error_data (*goose_schema_from_arrow)(goose_connection connection, struct ArrowSchema *schema,
	                                              goose_arrow_converted_schema *out_types);
	goose_error_data (*goose_data_chunk_from_arrow)(goose_connection connection, struct ArrowArray *arrow_array,
	                                                  goose_arrow_converted_schema converted_schema,
	                                                  goose_data_chunk *out_chunk);
	void (*goose_destroy_arrow_converted_schema)(goose_arrow_converted_schema *arrow_converted_schema);
	// New functions for interacting with catalog entries

	goose_catalog (*goose_client_context_get_catalog)(goose_client_context context, const char *catalog_name);
	const char *(*goose_catalog_get_type_name)(goose_catalog catalog);
	goose_catalog_entry (*goose_catalog_get_entry)(goose_catalog catalog, goose_client_context context,
	                                                 goose_catalog_entry_type entry_type, const char *schema_name,
	                                                 const char *entry_name);
	void (*goose_destroy_catalog)(goose_catalog *catalog);
	goose_catalog_entry_type (*goose_catalog_entry_get_type)(goose_catalog_entry entry);
	const char *(*goose_catalog_entry_get_name)(goose_catalog_entry entry);
	void (*goose_destroy_catalog_entry)(goose_catalog_entry *entry);
	// New configuration options functions

	goose_config_option (*goose_create_config_option)();
	void (*goose_destroy_config_option)(goose_config_option *option);
	void (*goose_config_option_set_name)(goose_config_option option, const char *name);
	void (*goose_config_option_set_type)(goose_config_option option, goose_logical_type type);
	void (*goose_config_option_set_default_value)(goose_config_option option, goose_value default_value);
	void (*goose_config_option_set_default_scope)(goose_config_option option,
	                                               goose_config_option_scope default_scope);
	void (*goose_config_option_set_description)(goose_config_option option, const char *description);
	goose_state (*goose_register_config_option)(goose_connection connection, goose_config_option option);
	goose_value (*goose_client_context_get_config_option)(goose_client_context context, const char *name,
	                                                        goose_config_option_scope *out_scope);
	// API to define custom copy functions

	goose_copy_function (*goose_create_copy_function)();
	void (*goose_copy_function_set_name)(goose_copy_function copy_function, const char *name);
	void (*goose_copy_function_set_extra_info)(goose_copy_function copy_function, void *extra_info,
	                                            goose_delete_callback_t destructor);
	goose_state (*goose_register_copy_function)(goose_connection connection, goose_copy_function copy_function);
	void (*goose_destroy_copy_function)(goose_copy_function *copy_function);
	void (*goose_copy_function_set_bind)(goose_copy_function copy_function, goose_copy_function_bind_t bind);
	void (*goose_copy_function_bind_set_error)(goose_copy_function_bind_info info, const char *error);
	void *(*goose_copy_function_bind_get_extra_info)(goose_copy_function_bind_info info);
	goose_client_context (*goose_copy_function_bind_get_client_context)(goose_copy_function_bind_info info);
	idx_t (*goose_copy_function_bind_get_column_count)(goose_copy_function_bind_info info);
	goose_logical_type (*goose_copy_function_bind_get_column_type)(goose_copy_function_bind_info info,
	                                                                 idx_t col_idx);
	goose_value (*goose_copy_function_bind_get_options)(goose_copy_function_bind_info info);
	void (*goose_copy_function_bind_set_bind_data)(goose_copy_function_bind_info info, void *bind_data,
	                                                goose_delete_callback_t destructor);
	void (*goose_copy_function_set_global_init)(goose_copy_function copy_function,
	                                             goose_copy_function_global_init_t init);
	void (*goose_copy_function_global_init_set_error)(goose_copy_function_global_init_info info, const char *error);
	void *(*goose_copy_function_global_init_get_extra_info)(goose_copy_function_global_init_info info);
	goose_client_context (*goose_copy_function_global_init_get_client_context)(
	    goose_copy_function_global_init_info info);
	void *(*goose_copy_function_global_init_get_bind_data)(goose_copy_function_global_init_info info);
	void (*goose_copy_function_global_init_set_global_state)(goose_copy_function_global_init_info info,
	                                                          void *global_state, goose_delete_callback_t destructor);
	const char *(*goose_copy_function_global_init_get_file_path)(goose_copy_function_global_init_info info);
	void (*goose_copy_function_set_sink)(goose_copy_function copy_function, goose_copy_function_sink_t function);
	void (*goose_copy_function_sink_set_error)(goose_copy_function_sink_info info, const char *error);
	void *(*goose_copy_function_sink_get_extra_info)(goose_copy_function_sink_info info);
	goose_client_context (*goose_copy_function_sink_get_client_context)(goose_copy_function_sink_info info);
	void *(*goose_copy_function_sink_get_bind_data)(goose_copy_function_sink_info info);
	void *(*goose_copy_function_sink_get_global_state)(goose_copy_function_sink_info info);
	void (*goose_copy_function_set_finalize)(goose_copy_function copy_function,
	                                          goose_copy_function_finalize_t finalize);
	void (*goose_copy_function_finalize_set_error)(goose_copy_function_finalize_info info, const char *error);
	void *(*goose_copy_function_finalize_get_extra_info)(goose_copy_function_finalize_info info);
	goose_client_context (*goose_copy_function_finalize_get_client_context)(goose_copy_function_finalize_info info);
	void *(*goose_copy_function_finalize_get_bind_data)(goose_copy_function_finalize_info info);
	void *(*goose_copy_function_finalize_get_global_state)(goose_copy_function_finalize_info info);
	void (*goose_copy_function_set_copy_from_function)(goose_copy_function copy_function,
	                                                    goose_table_function table_function);
	idx_t (*goose_table_function_bind_get_result_column_count)(goose_bind_info info);
	const char *(*goose_table_function_bind_get_result_column_name)(goose_bind_info info, idx_t col_idx);
	goose_logical_type (*goose_table_function_bind_get_result_column_type)(goose_bind_info info, idx_t col_idx);
	// New functions for goose error data

	goose_error_data (*goose_create_error_data)(goose_error_type type, const char *message);
	void (*goose_destroy_error_data)(goose_error_data *error_data);
	goose_error_type (*goose_error_data_error_type)(goose_error_data error_data);
	const char *(*goose_error_data_message)(goose_error_data error_data);
	bool (*goose_error_data_has_error)(goose_error_data error_data);
	// API to create and manipulate expressions

	void (*goose_destroy_expression)(goose_expression *expr);
	goose_logical_type (*goose_expression_return_type)(goose_expression expr);
	bool (*goose_expression_is_foldable)(goose_expression expr);
	goose_error_data (*goose_expression_fold)(goose_client_context context, goose_expression expr,
	                                            goose_value *out_value);
	// API to manage file system operations

	goose_file_system (*goose_client_context_get_file_system)(goose_client_context context);
	void (*goose_destroy_file_system)(goose_file_system *file_system);
	goose_state (*goose_file_system_open)(goose_file_system file_system, const char *path,
	                                        goose_file_open_options options, goose_file_handle *out_file);
	goose_error_data (*goose_file_system_error_data)(goose_file_system file_system);
	goose_file_open_options (*goose_create_file_open_options)();
	goose_state (*goose_file_open_options_set_flag)(goose_file_open_options options, goose_file_flag flag,
	                                                  bool value);
	void (*goose_destroy_file_open_options)(goose_file_open_options *options);
	void (*goose_destroy_file_handle)(goose_file_handle *file_handle);
	goose_error_data (*goose_file_handle_error_data)(goose_file_handle file_handle);
	goose_state (*goose_file_handle_close)(goose_file_handle file_handle);
	int64_t (*goose_file_handle_read)(goose_file_handle file_handle, void *buffer, int64_t size);
	int64_t (*goose_file_handle_write)(goose_file_handle file_handle, const void *buffer, int64_t size);
	goose_state (*goose_file_handle_seek)(goose_file_handle file_handle, int64_t position);
	int64_t (*goose_file_handle_tell)(goose_file_handle file_handle);
	goose_state (*goose_file_handle_sync)(goose_file_handle file_handle);
	int64_t (*goose_file_handle_size)(goose_file_handle file_handle);
	// API to register a custom log storage.

	goose_log_storage (*goose_create_log_storage)();
	void (*goose_destroy_log_storage)(goose_log_storage *log_storage);
	void (*goose_log_storage_set_write_log_entry)(goose_log_storage log_storage,
	                                               goose_logger_write_log_entry_t function);
	void (*goose_log_storage_set_extra_data)(goose_log_storage log_storage, void *extra_data,
	                                          goose_delete_callback_t delete_callback);
	void (*goose_log_storage_set_name)(goose_log_storage log_storage, const char *name);
	goose_state (*goose_register_log_storage)(goose_database database, goose_log_storage log_storage);
	// New functions around the client context

	idx_t (*goose_client_context_get_connection_id)(goose_client_context context);
	void (*goose_destroy_client_context)(goose_client_context *context);
	void (*goose_connection_get_client_context)(goose_connection connection, goose_client_context *out_context);
	goose_value (*goose_get_table_names)(goose_connection connection, const char *query, bool qualified);
	void (*goose_connection_get_arrow_options)(goose_connection connection, goose_arrow_options *out_arrow_options);
	void (*goose_destroy_arrow_options)(goose_arrow_options *arrow_options);
	// API to get information about the results of a prepared statement

	idx_t (*goose_prepared_statement_column_count)(goose_prepared_statement prepared_statement);
	const char *(*goose_prepared_statement_column_name)(goose_prepared_statement prepared_statement, idx_t col_idx);
	goose_logical_type (*goose_prepared_statement_column_logical_type)(goose_prepared_statement prepared_statement,
	                                                                     idx_t col_idx);
	goose_type (*goose_prepared_statement_column_type)(goose_prepared_statement prepared_statement, idx_t col_idx);
	// New query execution functions

	goose_arrow_options (*goose_result_get_arrow_options)(goose_result *result);
	// New functions around scalar function binding

	void (*goose_scalar_function_set_bind)(goose_scalar_function scalar_function, goose_scalar_function_bind_t bind);
	void (*goose_scalar_function_bind_set_error)(goose_bind_info info, const char *error);
	void (*goose_scalar_function_get_client_context)(goose_bind_info info, goose_client_context *out_context);
	void (*goose_scalar_function_set_bind_data)(goose_bind_info info, void *bind_data,
	                                             goose_delete_callback_t destroy);
	void *(*goose_scalar_function_get_bind_data)(goose_function_info info);
	void *(*goose_scalar_function_bind_get_extra_info)(goose_bind_info info);
	idx_t (*goose_scalar_function_bind_get_argument_count)(goose_bind_info info);
	goose_expression (*goose_scalar_function_bind_get_argument)(goose_bind_info info, idx_t index);
	void (*goose_scalar_function_set_bind_data_copy)(goose_bind_info info, goose_copy_callback_t copy);
	// New functions to configure local states for scalar functions

	void *(*goose_scalar_function_get_state)(goose_function_info info);
	void (*goose_scalar_function_set_init)(goose_scalar_function scalar_function, goose_scalar_function_init_t init);
	void (*goose_scalar_function_init_set_error)(goose_init_info info, const char *error);
	void (*goose_scalar_function_init_set_state)(goose_init_info info, void *state, goose_delete_callback_t destroy);
	void (*goose_scalar_function_init_get_client_context)(goose_init_info info, goose_client_context *out_context);
	void *(*goose_scalar_function_init_get_bind_data)(goose_init_info info);
	void *(*goose_scalar_function_init_get_extra_info)(goose_init_info info);
	// New string functions that are added

	char *(*goose_value_to_string)(goose_value value);
	// New functions around the table description

	idx_t (*goose_table_description_get_column_count)(goose_table_description table_description);
	goose_logical_type (*goose_table_description_get_column_type)(goose_table_description table_description,
	                                                                idx_t index);
	// New functions around table function binding

	void (*goose_table_function_get_client_context)(goose_bind_info info, goose_client_context *out_context);
	// New value functions that are added

	goose_value (*goose_create_map_value)(goose_logical_type map_type, goose_value *keys, goose_value *values,
	                                        idx_t entry_count);
	goose_value (*goose_create_union_value)(goose_logical_type union_type, idx_t tag_index, goose_value value);
	goose_value (*goose_create_time_ns)(goose_time_ns input);
	goose_time_ns (*goose_get_time_ns)(goose_value val);
	// API to create and manipulate vector types

	goose_vector (*goose_create_vector)(goose_logical_type type, idx_t capacity);
	void (*goose_destroy_vector)(goose_vector *vector);
	void (*goose_slice_vector)(goose_vector vector, goose_selection_vector sel, idx_t len);
	void (*goose_vector_reference_value)(goose_vector vector, goose_value value);
	void (*goose_vector_reference_vector)(goose_vector to_vector, goose_vector from_vector);
	goose_selection_vector (*goose_create_selection_vector)(idx_t size);
	void (*goose_destroy_selection_vector)(goose_selection_vector sel);
	sel_t *(*goose_selection_vector_get_data_ptr)(goose_selection_vector sel);
	void (*goose_vector_copy_sel)(goose_vector src, goose_vector dst, goose_selection_vector sel, idx_t src_count,
	                               idx_t src_offset, idx_t dst_offset);
} goose_ext_api_v1;

//===--------------------------------------------------------------------===//
// Struct Create Method
//===--------------------------------------------------------------------===//
inline goose_ext_api_v1 CreateAPIv1() {
	goose_ext_api_v1 result;
	result.goose_open = goose_open;
	result.goose_open_ext = goose_open_ext;
	result.goose_close = goose_close;
	result.goose_connect = goose_connect;
	result.goose_interrupt = goose_interrupt;
	result.goose_query_progress = goose_query_progress;
	result.goose_disconnect = goose_disconnect;
	result.goose_library_version = goose_library_version;
	result.goose_create_config = goose_create_config;
	result.goose_config_count = goose_config_count;
	result.goose_get_config_flag = goose_get_config_flag;
	result.goose_set_config = goose_set_config;
	result.goose_destroy_config = goose_destroy_config;
	result.goose_query = goose_query;
	result.goose_destroy_result = goose_destroy_result;
	result.goose_column_name = goose_column_name;
	result.goose_column_type = goose_column_type;
	result.goose_result_statement_type = goose_result_statement_type;
	result.goose_column_logical_type = goose_column_logical_type;
	result.goose_column_count = goose_column_count;
	result.goose_rows_changed = goose_rows_changed;
	result.goose_result_error = goose_result_error;
	result.goose_result_error_type = goose_result_error_type;
	result.goose_result_return_type = goose_result_return_type;
	result.goose_malloc = goose_malloc;
	result.goose_free = goose_free;
	result.goose_vector_size = goose_vector_size;
	result.goose_string_is_inlined = goose_string_is_inlined;
	result.goose_string_t_length = goose_string_t_length;
	result.goose_string_t_data = goose_string_t_data;
	result.goose_from_date = goose_from_date;
	result.goose_to_date = goose_to_date;
	result.goose_is_finite_date = goose_is_finite_date;
	result.goose_from_time = goose_from_time;
	result.goose_create_time_tz = goose_create_time_tz;
	result.goose_from_time_tz = goose_from_time_tz;
	result.goose_to_time = goose_to_time;
	result.goose_from_timestamp = goose_from_timestamp;
	result.goose_to_timestamp = goose_to_timestamp;
	result.goose_is_finite_timestamp = goose_is_finite_timestamp;
	result.goose_hugeint_to_double = goose_hugeint_to_double;
	result.goose_double_to_hugeint = goose_double_to_hugeint;
	result.goose_uhugeint_to_double = goose_uhugeint_to_double;
	result.goose_double_to_uhugeint = goose_double_to_uhugeint;
	result.goose_double_to_decimal = goose_double_to_decimal;
	result.goose_decimal_to_double = goose_decimal_to_double;
	result.goose_prepare = goose_prepare;
	result.goose_destroy_prepare = goose_destroy_prepare;
	result.goose_prepare_error = goose_prepare_error;
	result.goose_nparams = goose_nparams;
	result.goose_parameter_name = goose_parameter_name;
	result.goose_param_type = goose_param_type;
	result.goose_param_logical_type = goose_param_logical_type;
	result.goose_clear_bindings = goose_clear_bindings;
	result.goose_prepared_statement_type = goose_prepared_statement_type;
	result.goose_bind_value = goose_bind_value;
	result.goose_bind_parameter_index = goose_bind_parameter_index;
	result.goose_bind_boolean = goose_bind_boolean;
	result.goose_bind_int8 = goose_bind_int8;
	result.goose_bind_int16 = goose_bind_int16;
	result.goose_bind_int32 = goose_bind_int32;
	result.goose_bind_int64 = goose_bind_int64;
	result.goose_bind_hugeint = goose_bind_hugeint;
	result.goose_bind_uhugeint = goose_bind_uhugeint;
	result.goose_bind_decimal = goose_bind_decimal;
	result.goose_bind_uint8 = goose_bind_uint8;
	result.goose_bind_uint16 = goose_bind_uint16;
	result.goose_bind_uint32 = goose_bind_uint32;
	result.goose_bind_uint64 = goose_bind_uint64;
	result.goose_bind_float = goose_bind_float;
	result.goose_bind_double = goose_bind_double;
	result.goose_bind_date = goose_bind_date;
	result.goose_bind_time = goose_bind_time;
	result.goose_bind_timestamp = goose_bind_timestamp;
	result.goose_bind_timestamp_tz = goose_bind_timestamp_tz;
	result.goose_bind_interval = goose_bind_interval;
	result.goose_bind_varchar = goose_bind_varchar;
	result.goose_bind_varchar_length = goose_bind_varchar_length;
	result.goose_bind_blob = goose_bind_blob;
	result.goose_bind_null = goose_bind_null;
	result.goose_execute_prepared = goose_execute_prepared;
	result.goose_extract_statements = goose_extract_statements;
	result.goose_prepare_extracted_statement = goose_prepare_extracted_statement;
	result.goose_extract_statements_error = goose_extract_statements_error;
	result.goose_destroy_extracted = goose_destroy_extracted;
	result.goose_pending_prepared = goose_pending_prepared;
	result.goose_destroy_pending = goose_destroy_pending;
	result.goose_pending_error = goose_pending_error;
	result.goose_pending_execute_task = goose_pending_execute_task;
	result.goose_pending_execute_check_state = goose_pending_execute_check_state;
	result.goose_execute_pending = goose_execute_pending;
	result.goose_pending_execution_is_finished = goose_pending_execution_is_finished;
	result.goose_destroy_value = goose_destroy_value;
	result.goose_create_varchar = goose_create_varchar;
	result.goose_create_varchar_length = goose_create_varchar_length;
	result.goose_create_bool = goose_create_bool;
	result.goose_create_int8 = goose_create_int8;
	result.goose_create_uint8 = goose_create_uint8;
	result.goose_create_int16 = goose_create_int16;
	result.goose_create_uint16 = goose_create_uint16;
	result.goose_create_int32 = goose_create_int32;
	result.goose_create_uint32 = goose_create_uint32;
	result.goose_create_uint64 = goose_create_uint64;
	result.goose_create_int64 = goose_create_int64;
	result.goose_create_hugeint = goose_create_hugeint;
	result.goose_create_uhugeint = goose_create_uhugeint;
	result.goose_create_float = goose_create_float;
	result.goose_create_double = goose_create_double;
	result.goose_create_date = goose_create_date;
	result.goose_create_time = goose_create_time;
	result.goose_create_time_tz_value = goose_create_time_tz_value;
	result.goose_create_timestamp = goose_create_timestamp;
	result.goose_create_interval = goose_create_interval;
	result.goose_create_blob = goose_create_blob;
	result.goose_create_bignum = goose_create_bignum;
	result.goose_create_decimal = goose_create_decimal;
	result.goose_create_bit = goose_create_bit;
	result.goose_create_uuid = goose_create_uuid;
	result.goose_get_bool = goose_get_bool;
	result.goose_get_int8 = goose_get_int8;
	result.goose_get_uint8 = goose_get_uint8;
	result.goose_get_int16 = goose_get_int16;
	result.goose_get_uint16 = goose_get_uint16;
	result.goose_get_int32 = goose_get_int32;
	result.goose_get_uint32 = goose_get_uint32;
	result.goose_get_int64 = goose_get_int64;
	result.goose_get_uint64 = goose_get_uint64;
	result.goose_get_hugeint = goose_get_hugeint;
	result.goose_get_uhugeint = goose_get_uhugeint;
	result.goose_get_float = goose_get_float;
	result.goose_get_double = goose_get_double;
	result.goose_get_date = goose_get_date;
	result.goose_get_time = goose_get_time;
	result.goose_get_time_tz = goose_get_time_tz;
	result.goose_get_timestamp = goose_get_timestamp;
	result.goose_get_interval = goose_get_interval;
	result.goose_get_value_type = goose_get_value_type;
	result.goose_get_blob = goose_get_blob;
	result.goose_get_bignum = goose_get_bignum;
	result.goose_get_decimal = goose_get_decimal;
	result.goose_get_bit = goose_get_bit;
	result.goose_get_uuid = goose_get_uuid;
	result.goose_get_varchar = goose_get_varchar;
	result.goose_create_struct_value = goose_create_struct_value;
	result.goose_create_list_value = goose_create_list_value;
	result.goose_create_array_value = goose_create_array_value;
	result.goose_get_map_size = goose_get_map_size;
	result.goose_get_map_key = goose_get_map_key;
	result.goose_get_map_value = goose_get_map_value;
	result.goose_is_null_value = goose_is_null_value;
	result.goose_create_null_value = goose_create_null_value;
	result.goose_get_list_size = goose_get_list_size;
	result.goose_get_list_child = goose_get_list_child;
	result.goose_create_enum_value = goose_create_enum_value;
	result.goose_get_enum_value = goose_get_enum_value;
	result.goose_get_struct_child = goose_get_struct_child;
	result.goose_create_logical_type = goose_create_logical_type;
	result.goose_logical_type_get_alias = goose_logical_type_get_alias;
	result.goose_logical_type_set_alias = goose_logical_type_set_alias;
	result.goose_create_list_type = goose_create_list_type;
	result.goose_create_array_type = goose_create_array_type;
	result.goose_create_map_type = goose_create_map_type;
	result.goose_create_union_type = goose_create_union_type;
	result.goose_create_struct_type = goose_create_struct_type;
	result.goose_create_enum_type = goose_create_enum_type;
	result.goose_create_decimal_type = goose_create_decimal_type;
	result.goose_get_type_id = goose_get_type_id;
	result.goose_decimal_width = goose_decimal_width;
	result.goose_decimal_scale = goose_decimal_scale;
	result.goose_decimal_internal_type = goose_decimal_internal_type;
	result.goose_enum_internal_type = goose_enum_internal_type;
	result.goose_enum_dictionary_size = goose_enum_dictionary_size;
	result.goose_enum_dictionary_value = goose_enum_dictionary_value;
	result.goose_list_type_child_type = goose_list_type_child_type;
	result.goose_array_type_child_type = goose_array_type_child_type;
	result.goose_array_type_array_size = goose_array_type_array_size;
	result.goose_map_type_key_type = goose_map_type_key_type;
	result.goose_map_type_value_type = goose_map_type_value_type;
	result.goose_struct_type_child_count = goose_struct_type_child_count;
	result.goose_struct_type_child_name = goose_struct_type_child_name;
	result.goose_struct_type_child_type = goose_struct_type_child_type;
	result.goose_union_type_member_count = goose_union_type_member_count;
	result.goose_union_type_member_name = goose_union_type_member_name;
	result.goose_union_type_member_type = goose_union_type_member_type;
	result.goose_destroy_logical_type = goose_destroy_logical_type;
	result.goose_register_logical_type = goose_register_logical_type;
	result.goose_create_data_chunk = goose_create_data_chunk;
	result.goose_destroy_data_chunk = goose_destroy_data_chunk;
	result.goose_data_chunk_reset = goose_data_chunk_reset;
	result.goose_data_chunk_get_column_count = goose_data_chunk_get_column_count;
	result.goose_data_chunk_get_vector = goose_data_chunk_get_vector;
	result.goose_data_chunk_get_size = goose_data_chunk_get_size;
	result.goose_data_chunk_set_size = goose_data_chunk_set_size;
	result.goose_vector_get_column_type = goose_vector_get_column_type;
	result.goose_vector_get_data = goose_vector_get_data;
	result.goose_vector_get_validity = goose_vector_get_validity;
	result.goose_vector_ensure_validity_writable = goose_vector_ensure_validity_writable;
	result.goose_vector_assign_string_element = goose_vector_assign_string_element;
	result.goose_vector_assign_string_element_len = goose_vector_assign_string_element_len;
	result.goose_list_vector_get_child = goose_list_vector_get_child;
	result.goose_list_vector_get_size = goose_list_vector_get_size;
	result.goose_list_vector_set_size = goose_list_vector_set_size;
	result.goose_list_vector_reserve = goose_list_vector_reserve;
	result.goose_struct_vector_get_child = goose_struct_vector_get_child;
	result.goose_array_vector_get_child = goose_array_vector_get_child;
	result.goose_validity_row_is_valid = goose_validity_row_is_valid;
	result.goose_validity_set_row_validity = goose_validity_set_row_validity;
	result.goose_validity_set_row_invalid = goose_validity_set_row_invalid;
	result.goose_validity_set_row_valid = goose_validity_set_row_valid;
	result.goose_create_scalar_function = goose_create_scalar_function;
	result.goose_destroy_scalar_function = goose_destroy_scalar_function;
	result.goose_scalar_function_set_name = goose_scalar_function_set_name;
	result.goose_scalar_function_set_varargs = goose_scalar_function_set_varargs;
	result.goose_scalar_function_set_special_handling = goose_scalar_function_set_special_handling;
	result.goose_scalar_function_set_volatile = goose_scalar_function_set_volatile;
	result.goose_scalar_function_add_parameter = goose_scalar_function_add_parameter;
	result.goose_scalar_function_set_return_type = goose_scalar_function_set_return_type;
	result.goose_scalar_function_set_extra_info = goose_scalar_function_set_extra_info;
	result.goose_scalar_function_set_function = goose_scalar_function_set_function;
	result.goose_register_scalar_function = goose_register_scalar_function;
	result.goose_scalar_function_get_extra_info = goose_scalar_function_get_extra_info;
	result.goose_scalar_function_set_error = goose_scalar_function_set_error;
	result.goose_create_scalar_function_set = goose_create_scalar_function_set;
	result.goose_destroy_scalar_function_set = goose_destroy_scalar_function_set;
	result.goose_add_scalar_function_to_set = goose_add_scalar_function_to_set;
	result.goose_register_scalar_function_set = goose_register_scalar_function_set;
	result.goose_create_aggregate_function = goose_create_aggregate_function;
	result.goose_destroy_aggregate_function = goose_destroy_aggregate_function;
	result.goose_aggregate_function_set_name = goose_aggregate_function_set_name;
	result.goose_aggregate_function_add_parameter = goose_aggregate_function_add_parameter;
	result.goose_aggregate_function_set_return_type = goose_aggregate_function_set_return_type;
	result.goose_aggregate_function_set_functions = goose_aggregate_function_set_functions;
	result.goose_aggregate_function_set_destructor = goose_aggregate_function_set_destructor;
	result.goose_register_aggregate_function = goose_register_aggregate_function;
	result.goose_aggregate_function_set_special_handling = goose_aggregate_function_set_special_handling;
	result.goose_aggregate_function_set_extra_info = goose_aggregate_function_set_extra_info;
	result.goose_aggregate_function_get_extra_info = goose_aggregate_function_get_extra_info;
	result.goose_aggregate_function_set_error = goose_aggregate_function_set_error;
	result.goose_create_aggregate_function_set = goose_create_aggregate_function_set;
	result.goose_destroy_aggregate_function_set = goose_destroy_aggregate_function_set;
	result.goose_add_aggregate_function_to_set = goose_add_aggregate_function_to_set;
	result.goose_register_aggregate_function_set = goose_register_aggregate_function_set;
	result.goose_create_table_function = goose_create_table_function;
	result.goose_destroy_table_function = goose_destroy_table_function;
	result.goose_table_function_set_name = goose_table_function_set_name;
	result.goose_table_function_add_parameter = goose_table_function_add_parameter;
	result.goose_table_function_add_named_parameter = goose_table_function_add_named_parameter;
	result.goose_table_function_set_extra_info = goose_table_function_set_extra_info;
	result.goose_table_function_set_bind = goose_table_function_set_bind;
	result.goose_table_function_set_init = goose_table_function_set_init;
	result.goose_table_function_set_local_init = goose_table_function_set_local_init;
	result.goose_table_function_set_function = goose_table_function_set_function;
	result.goose_table_function_supports_projection_pushdown = goose_table_function_supports_projection_pushdown;
	result.goose_register_table_function = goose_register_table_function;
	result.goose_bind_get_extra_info = goose_bind_get_extra_info;
	result.goose_bind_add_result_column = goose_bind_add_result_column;
	result.goose_bind_get_parameter_count = goose_bind_get_parameter_count;
	result.goose_bind_get_parameter = goose_bind_get_parameter;
	result.goose_bind_get_named_parameter = goose_bind_get_named_parameter;
	result.goose_bind_set_bind_data = goose_bind_set_bind_data;
	result.goose_bind_set_cardinality = goose_bind_set_cardinality;
	result.goose_bind_set_error = goose_bind_set_error;
	result.goose_init_get_extra_info = goose_init_get_extra_info;
	result.goose_init_get_bind_data = goose_init_get_bind_data;
	result.goose_init_set_init_data = goose_init_set_init_data;
	result.goose_init_get_column_count = goose_init_get_column_count;
	result.goose_init_get_column_index = goose_init_get_column_index;
	result.goose_init_set_max_threads = goose_init_set_max_threads;
	result.goose_init_set_error = goose_init_set_error;
	result.goose_function_get_extra_info = goose_function_get_extra_info;
	result.goose_function_get_bind_data = goose_function_get_bind_data;
	result.goose_function_get_init_data = goose_function_get_init_data;
	result.goose_function_get_local_init_data = goose_function_get_local_init_data;
	result.goose_function_set_error = goose_function_set_error;
	result.goose_add_replacement_scan = goose_add_replacement_scan;
	result.goose_replacement_scan_set_function_name = goose_replacement_scan_set_function_name;
	result.goose_replacement_scan_add_parameter = goose_replacement_scan_add_parameter;
	result.goose_replacement_scan_set_error = goose_replacement_scan_set_error;
	result.goose_profiling_info_get_metrics = goose_profiling_info_get_metrics;
	result.goose_profiling_info_get_child_count = goose_profiling_info_get_child_count;
	result.goose_profiling_info_get_child = goose_profiling_info_get_child;
	result.goose_appender_create = goose_appender_create;
	result.goose_appender_create_ext = goose_appender_create_ext;
	result.goose_appender_column_count = goose_appender_column_count;
	result.goose_appender_column_type = goose_appender_column_type;
	result.goose_appender_error = goose_appender_error;
	result.goose_appender_flush = goose_appender_flush;
	result.goose_appender_close = goose_appender_close;
	result.goose_appender_destroy = goose_appender_destroy;
	result.goose_appender_add_column = goose_appender_add_column;
	result.goose_appender_clear_columns = goose_appender_clear_columns;
	result.goose_append_data_chunk = goose_append_data_chunk;
	result.goose_table_description_create = goose_table_description_create;
	result.goose_table_description_create_ext = goose_table_description_create_ext;
	result.goose_table_description_destroy = goose_table_description_destroy;
	result.goose_table_description_error = goose_table_description_error;
	result.goose_column_has_default = goose_column_has_default;
	result.goose_table_description_get_column_name = goose_table_description_get_column_name;
	result.goose_execute_tasks = goose_execute_tasks;
	result.goose_create_task_state = goose_create_task_state;
	result.goose_execute_tasks_state = goose_execute_tasks_state;
	result.goose_execute_n_tasks_state = goose_execute_n_tasks_state;
	result.goose_finish_execution = goose_finish_execution;
	result.goose_task_state_is_finished = goose_task_state_is_finished;
	result.goose_destroy_task_state = goose_destroy_task_state;
	result.goose_execution_is_finished = goose_execution_is_finished;
	result.goose_fetch_chunk = goose_fetch_chunk;
	result.goose_create_cast_function = goose_create_cast_function;
	result.goose_cast_function_set_source_type = goose_cast_function_set_source_type;
	result.goose_cast_function_set_target_type = goose_cast_function_set_target_type;
	result.goose_cast_function_set_implicit_cast_cost = goose_cast_function_set_implicit_cast_cost;
	result.goose_cast_function_set_function = goose_cast_function_set_function;
	result.goose_cast_function_set_extra_info = goose_cast_function_set_extra_info;
	result.goose_cast_function_get_extra_info = goose_cast_function_get_extra_info;
	result.goose_cast_function_get_cast_mode = goose_cast_function_get_cast_mode;
	result.goose_cast_function_set_error = goose_cast_function_set_error;
	result.goose_cast_function_set_row_error = goose_cast_function_set_row_error;
	result.goose_register_cast_function = goose_register_cast_function;
	result.goose_destroy_cast_function = goose_destroy_cast_function;
	result.goose_is_finite_timestamp_s = goose_is_finite_timestamp_s;
	result.goose_is_finite_timestamp_ms = goose_is_finite_timestamp_ms;
	result.goose_is_finite_timestamp_ns = goose_is_finite_timestamp_ns;
	result.goose_create_timestamp_tz = goose_create_timestamp_tz;
	result.goose_create_timestamp_s = goose_create_timestamp_s;
	result.goose_create_timestamp_ms = goose_create_timestamp_ms;
	result.goose_create_timestamp_ns = goose_create_timestamp_ns;
	result.goose_get_timestamp_tz = goose_get_timestamp_tz;
	result.goose_get_timestamp_s = goose_get_timestamp_s;
	result.goose_get_timestamp_ms = goose_get_timestamp_ms;
	result.goose_get_timestamp_ns = goose_get_timestamp_ns;
	result.goose_append_value = goose_append_value;
	result.goose_get_profiling_info = goose_get_profiling_info;
	result.goose_profiling_info_get_value = goose_profiling_info_get_value;
	result.goose_appender_begin_row = goose_appender_begin_row;
	result.goose_appender_end_row = goose_appender_end_row;
	result.goose_append_default = goose_append_default;
	result.goose_append_bool = goose_append_bool;
	result.goose_append_int8 = goose_append_int8;
	result.goose_append_int16 = goose_append_int16;
	result.goose_append_int32 = goose_append_int32;
	result.goose_append_int64 = goose_append_int64;
	result.goose_append_hugeint = goose_append_hugeint;
	result.goose_append_uint8 = goose_append_uint8;
	result.goose_append_uint16 = goose_append_uint16;
	result.goose_append_uint32 = goose_append_uint32;
	result.goose_append_uint64 = goose_append_uint64;
	result.goose_append_uhugeint = goose_append_uhugeint;
	result.goose_append_float = goose_append_float;
	result.goose_append_double = goose_append_double;
	result.goose_append_date = goose_append_date;
	result.goose_append_time = goose_append_time;
	result.goose_append_timestamp = goose_append_timestamp;
	result.goose_append_interval = goose_append_interval;
	result.goose_append_varchar = goose_append_varchar;
	result.goose_append_varchar_length = goose_append_varchar_length;
	result.goose_append_blob = goose_append_blob;
	result.goose_append_null = goose_append_null;
	result.goose_row_count = goose_row_count;
	result.goose_column_data = goose_column_data;
	result.goose_nullmask_data = goose_nullmask_data;
	result.goose_result_get_chunk = goose_result_get_chunk;
	result.goose_result_is_streaming = goose_result_is_streaming;
	result.goose_result_chunk_count = goose_result_chunk_count;
	result.goose_value_boolean = goose_value_boolean;
	result.goose_value_int8 = goose_value_int8;
	result.goose_value_int16 = goose_value_int16;
	result.goose_value_int32 = goose_value_int32;
	result.goose_value_int64 = goose_value_int64;
	result.goose_value_hugeint = goose_value_hugeint;
	result.goose_value_uhugeint = goose_value_uhugeint;
	result.goose_value_decimal = goose_value_decimal;
	result.goose_value_uint8 = goose_value_uint8;
	result.goose_value_uint16 = goose_value_uint16;
	result.goose_value_uint32 = goose_value_uint32;
	result.goose_value_uint64 = goose_value_uint64;
	result.goose_value_float = goose_value_float;
	result.goose_value_double = goose_value_double;
	result.goose_value_date = goose_value_date;
	result.goose_value_time = goose_value_time;
	result.goose_value_timestamp = goose_value_timestamp;
	result.goose_value_interval = goose_value_interval;
	result.goose_value_varchar = goose_value_varchar;
	result.goose_value_string = goose_value_string;
	result.goose_value_varchar_internal = goose_value_varchar_internal;
	result.goose_value_string_internal = goose_value_string_internal;
	result.goose_value_blob = goose_value_blob;
	result.goose_value_is_null = goose_value_is_null;
	result.goose_execute_prepared_streaming = goose_execute_prepared_streaming;
	result.goose_pending_prepared_streaming = goose_pending_prepared_streaming;
	result.goose_query_arrow = goose_query_arrow;
	result.goose_query_arrow_schema = goose_query_arrow_schema;
	result.goose_prepared_arrow_schema = goose_prepared_arrow_schema;
	result.goose_result_arrow_array = goose_result_arrow_array;
	result.goose_query_arrow_array = goose_query_arrow_array;
	result.goose_arrow_column_count = goose_arrow_column_count;
	result.goose_arrow_row_count = goose_arrow_row_count;
	result.goose_arrow_rows_changed = goose_arrow_rows_changed;
	result.goose_query_arrow_error = goose_query_arrow_error;
	result.goose_destroy_arrow = goose_destroy_arrow;
	result.goose_destroy_arrow_stream = goose_destroy_arrow_stream;
	result.goose_execute_prepared_arrow = goose_execute_prepared_arrow;
	result.goose_arrow_scan = goose_arrow_scan;
	result.goose_arrow_array_scan = goose_arrow_array_scan;
	result.goose_stream_fetch_chunk = goose_stream_fetch_chunk;
	result.goose_create_instance_cache = goose_create_instance_cache;
	result.goose_get_or_create_from_cache = goose_get_or_create_from_cache;
	result.goose_destroy_instance_cache = goose_destroy_instance_cache;
	result.goose_append_default_to_chunk = goose_append_default_to_chunk;
	result.goose_appender_error_data = goose_appender_error_data;
	result.goose_appender_create_query = goose_appender_create_query;
	result.goose_appender_clear = goose_appender_clear;
	result.goose_to_arrow_schema = goose_to_arrow_schema;
	result.goose_data_chunk_to_arrow = goose_data_chunk_to_arrow;
	result.goose_schema_from_arrow = goose_schema_from_arrow;
	result.goose_data_chunk_from_arrow = goose_data_chunk_from_arrow;
	result.goose_destroy_arrow_converted_schema = goose_destroy_arrow_converted_schema;
	result.goose_client_context_get_catalog = goose_client_context_get_catalog;
	result.goose_catalog_get_type_name = goose_catalog_get_type_name;
	result.goose_catalog_get_entry = goose_catalog_get_entry;
	result.goose_destroy_catalog = goose_destroy_catalog;
	result.goose_catalog_entry_get_type = goose_catalog_entry_get_type;
	result.goose_catalog_entry_get_name = goose_catalog_entry_get_name;
	result.goose_destroy_catalog_entry = goose_destroy_catalog_entry;
	result.goose_create_config_option = goose_create_config_option;
	result.goose_destroy_config_option = goose_destroy_config_option;
	result.goose_config_option_set_name = goose_config_option_set_name;
	result.goose_config_option_set_type = goose_config_option_set_type;
	result.goose_config_option_set_default_value = goose_config_option_set_default_value;
	result.goose_config_option_set_default_scope = goose_config_option_set_default_scope;
	result.goose_config_option_set_description = goose_config_option_set_description;
	result.goose_register_config_option = goose_register_config_option;
	result.goose_client_context_get_config_option = goose_client_context_get_config_option;
	result.goose_create_copy_function = goose_create_copy_function;
	result.goose_copy_function_set_name = goose_copy_function_set_name;
	result.goose_copy_function_set_extra_info = goose_copy_function_set_extra_info;
	result.goose_register_copy_function = goose_register_copy_function;
	result.goose_destroy_copy_function = goose_destroy_copy_function;
	result.goose_copy_function_set_bind = goose_copy_function_set_bind;
	result.goose_copy_function_bind_set_error = goose_copy_function_bind_set_error;
	result.goose_copy_function_bind_get_extra_info = goose_copy_function_bind_get_extra_info;
	result.goose_copy_function_bind_get_client_context = goose_copy_function_bind_get_client_context;
	result.goose_copy_function_bind_get_column_count = goose_copy_function_bind_get_column_count;
	result.goose_copy_function_bind_get_column_type = goose_copy_function_bind_get_column_type;
	result.goose_copy_function_bind_get_options = goose_copy_function_bind_get_options;
	result.goose_copy_function_bind_set_bind_data = goose_copy_function_bind_set_bind_data;
	result.goose_copy_function_set_global_init = goose_copy_function_set_global_init;
	result.goose_copy_function_global_init_set_error = goose_copy_function_global_init_set_error;
	result.goose_copy_function_global_init_get_extra_info = goose_copy_function_global_init_get_extra_info;
	result.goose_copy_function_global_init_get_client_context = goose_copy_function_global_init_get_client_context;
	result.goose_copy_function_global_init_get_bind_data = goose_copy_function_global_init_get_bind_data;
	result.goose_copy_function_global_init_set_global_state = goose_copy_function_global_init_set_global_state;
	result.goose_copy_function_global_init_get_file_path = goose_copy_function_global_init_get_file_path;
	result.goose_copy_function_set_sink = goose_copy_function_set_sink;
	result.goose_copy_function_sink_set_error = goose_copy_function_sink_set_error;
	result.goose_copy_function_sink_get_extra_info = goose_copy_function_sink_get_extra_info;
	result.goose_copy_function_sink_get_client_context = goose_copy_function_sink_get_client_context;
	result.goose_copy_function_sink_get_bind_data = goose_copy_function_sink_get_bind_data;
	result.goose_copy_function_sink_get_global_state = goose_copy_function_sink_get_global_state;
	result.goose_copy_function_set_finalize = goose_copy_function_set_finalize;
	result.goose_copy_function_finalize_set_error = goose_copy_function_finalize_set_error;
	result.goose_copy_function_finalize_get_extra_info = goose_copy_function_finalize_get_extra_info;
	result.goose_copy_function_finalize_get_client_context = goose_copy_function_finalize_get_client_context;
	result.goose_copy_function_finalize_get_bind_data = goose_copy_function_finalize_get_bind_data;
	result.goose_copy_function_finalize_get_global_state = goose_copy_function_finalize_get_global_state;
	result.goose_copy_function_set_copy_from_function = goose_copy_function_set_copy_from_function;
	result.goose_table_function_bind_get_result_column_count = goose_table_function_bind_get_result_column_count;
	result.goose_table_function_bind_get_result_column_name = goose_table_function_bind_get_result_column_name;
	result.goose_table_function_bind_get_result_column_type = goose_table_function_bind_get_result_column_type;
	result.goose_create_error_data = goose_create_error_data;
	result.goose_destroy_error_data = goose_destroy_error_data;
	result.goose_error_data_error_type = goose_error_data_error_type;
	result.goose_error_data_message = goose_error_data_message;
	result.goose_error_data_has_error = goose_error_data_has_error;
	result.goose_destroy_expression = goose_destroy_expression;
	result.goose_expression_return_type = goose_expression_return_type;
	result.goose_expression_is_foldable = goose_expression_is_foldable;
	result.goose_expression_fold = goose_expression_fold;
	result.goose_client_context_get_file_system = goose_client_context_get_file_system;
	result.goose_destroy_file_system = goose_destroy_file_system;
	result.goose_file_system_open = goose_file_system_open;
	result.goose_file_system_error_data = goose_file_system_error_data;
	result.goose_create_file_open_options = goose_create_file_open_options;
	result.goose_file_open_options_set_flag = goose_file_open_options_set_flag;
	result.goose_destroy_file_open_options = goose_destroy_file_open_options;
	result.goose_destroy_file_handle = goose_destroy_file_handle;
	result.goose_file_handle_error_data = goose_file_handle_error_data;
	result.goose_file_handle_close = goose_file_handle_close;
	result.goose_file_handle_read = goose_file_handle_read;
	result.goose_file_handle_write = goose_file_handle_write;
	result.goose_file_handle_seek = goose_file_handle_seek;
	result.goose_file_handle_tell = goose_file_handle_tell;
	result.goose_file_handle_sync = goose_file_handle_sync;
	result.goose_file_handle_size = goose_file_handle_size;
	result.goose_create_log_storage = goose_create_log_storage;
	result.goose_destroy_log_storage = goose_destroy_log_storage;
	result.goose_log_storage_set_write_log_entry = goose_log_storage_set_write_log_entry;
	result.goose_log_storage_set_extra_data = goose_log_storage_set_extra_data;
	result.goose_log_storage_set_name = goose_log_storage_set_name;
	result.goose_register_log_storage = goose_register_log_storage;
	result.goose_client_context_get_connection_id = goose_client_context_get_connection_id;
	result.goose_destroy_client_context = goose_destroy_client_context;
	result.goose_connection_get_client_context = goose_connection_get_client_context;
	result.goose_get_table_names = goose_get_table_names;
	result.goose_connection_get_arrow_options = goose_connection_get_arrow_options;
	result.goose_destroy_arrow_options = goose_destroy_arrow_options;
	result.goose_prepared_statement_column_count = goose_prepared_statement_column_count;
	result.goose_prepared_statement_column_name = goose_prepared_statement_column_name;
	result.goose_prepared_statement_column_logical_type = goose_prepared_statement_column_logical_type;
	result.goose_prepared_statement_column_type = goose_prepared_statement_column_type;
	result.goose_result_get_arrow_options = goose_result_get_arrow_options;
	result.goose_scalar_function_set_bind = goose_scalar_function_set_bind;
	result.goose_scalar_function_bind_set_error = goose_scalar_function_bind_set_error;
	result.goose_scalar_function_get_client_context = goose_scalar_function_get_client_context;
	result.goose_scalar_function_set_bind_data = goose_scalar_function_set_bind_data;
	result.goose_scalar_function_get_bind_data = goose_scalar_function_get_bind_data;
	result.goose_scalar_function_bind_get_extra_info = goose_scalar_function_bind_get_extra_info;
	result.goose_scalar_function_bind_get_argument_count = goose_scalar_function_bind_get_argument_count;
	result.goose_scalar_function_bind_get_argument = goose_scalar_function_bind_get_argument;
	result.goose_scalar_function_set_bind_data_copy = goose_scalar_function_set_bind_data_copy;
	result.goose_scalar_function_get_state = goose_scalar_function_get_state;
	result.goose_scalar_function_set_init = goose_scalar_function_set_init;
	result.goose_scalar_function_init_set_error = goose_scalar_function_init_set_error;
	result.goose_scalar_function_init_set_state = goose_scalar_function_init_set_state;
	result.goose_scalar_function_init_get_client_context = goose_scalar_function_init_get_client_context;
	result.goose_scalar_function_init_get_bind_data = goose_scalar_function_init_get_bind_data;
	result.goose_scalar_function_init_get_extra_info = goose_scalar_function_init_get_extra_info;
	result.goose_value_to_string = goose_value_to_string;
	result.goose_table_description_get_column_count = goose_table_description_get_column_count;
	result.goose_table_description_get_column_type = goose_table_description_get_column_type;
	result.goose_table_function_get_client_context = goose_table_function_get_client_context;
	result.goose_create_map_value = goose_create_map_value;
	result.goose_create_union_value = goose_create_union_value;
	result.goose_create_time_ns = goose_create_time_ns;
	result.goose_get_time_ns = goose_get_time_ns;
	result.goose_create_vector = goose_create_vector;
	result.goose_destroy_vector = goose_destroy_vector;
	result.goose_slice_vector = goose_slice_vector;
	result.goose_vector_reference_value = goose_vector_reference_value;
	result.goose_vector_reference_vector = goose_vector_reference_vector;
	result.goose_create_selection_vector = goose_create_selection_vector;
	result.goose_destroy_selection_vector = goose_destroy_selection_vector;
	result.goose_selection_vector_get_data_ptr = goose_selection_vector_get_data_ptr;
	result.goose_vector_copy_sel = goose_vector_copy_sel;
	return result;
}

#define GOOSE_EXTENSION_API_VERSION_MAJOR  1
#define GOOSE_EXTENSION_API_VERSION_MINOR  2
#define GOOSE_EXTENSION_API_VERSION_PATCH  0
#define GOOSE_EXTENSION_API_VERSION_STRING "v1.2.0"
