#include <goose/main/capi/capi_internal.h>
#include <goose/main/query_result.h>
#include <goose/main/pending_query_result.h>
#include <goose/common/error_data.h>
#include <goose/common/case_insensitive_map.h>
#include <goose/common/optional_ptr.h>

using goose::case_insensitive_map_t;
using goose::make_uniq;
using goose::optional_ptr;
using goose::PendingExecutionResult;
using goose::PendingQueryResult;
using goose::PendingStatementWrapper;
using goose::PreparedStatementWrapper;
using goose::Value;

goose_state goose_pending_prepared_internal(goose_prepared_statement prepared_statement,
                                              goose_pending_result *out_result, bool allow_streaming) {
	if (!prepared_statement || !out_result) {
		return GooseError;
	}
	auto wrapper = reinterpret_cast<PreparedStatementWrapper *>(prepared_statement);
	auto result = new PendingStatementWrapper();
	result->allow_streaming = allow_streaming;

	try {
		result->statement = wrapper->statement->PendingQuery(wrapper->values, allow_streaming);
	} catch (std::exception &ex) {
		result->statement = make_uniq<PendingQueryResult>(goose::ErrorData(ex));
	}
	goose_state return_value = !result->statement->HasError() ? GooseSuccess : GooseError;
	*out_result = reinterpret_cast<goose_pending_result>(result);

	return return_value;
}

goose_state goose_pending_prepared(goose_prepared_statement prepared_statement, goose_pending_result *out_result) {
	return goose_pending_prepared_internal(prepared_statement, out_result, false);
}

goose_state goose_pending_prepared_streaming(goose_prepared_statement prepared_statement,
                                               goose_pending_result *out_result) {
	return goose_pending_prepared_internal(prepared_statement, out_result, true);
}

void goose_destroy_pending(goose_pending_result *pending_result) {
	if (!pending_result || !*pending_result) {
		return;
	}
	auto wrapper = reinterpret_cast<PendingStatementWrapper *>(*pending_result);
	if (wrapper->statement) {
		wrapper->statement->Close();
	}
	delete wrapper;
	*pending_result = nullptr;
}

const char *goose_pending_error(goose_pending_result pending_result) {
	if (!pending_result) {
		return nullptr;
	}
	auto wrapper = reinterpret_cast<PendingStatementWrapper *>(pending_result);
	if (!wrapper->statement) {
		return nullptr;
	}
	return wrapper->statement->GetError().c_str();
}

goose_pending_state goose_pending_execute_check_state(goose_pending_result pending_result) {
	if (!pending_result) {
		return GOOSE_PENDING_ERROR;
	}
	auto wrapper = reinterpret_cast<PendingStatementWrapper *>(pending_result);
	if (!wrapper->statement) {
		return GOOSE_PENDING_ERROR;
	}
	if (wrapper->statement->HasError()) {
		return GOOSE_PENDING_ERROR;
	}
	PendingExecutionResult return_value;
	try {
		return_value = wrapper->statement->CheckPulse();
	} catch (std::exception &ex) {
		wrapper->statement->SetError(goose::ErrorData(ex));
		return GOOSE_PENDING_ERROR;
	}
	switch (return_value) {
	case PendingExecutionResult::BLOCKED:
	case PendingExecutionResult::RESULT_READY:
		return GOOSE_PENDING_RESULT_READY;
	case PendingExecutionResult::NO_TASKS_AVAILABLE:
		return GOOSE_PENDING_NO_TASKS_AVAILABLE;
	case PendingExecutionResult::RESULT_NOT_READY:
		return GOOSE_PENDING_RESULT_NOT_READY;
	default:
		return GOOSE_PENDING_ERROR;
	}
}

goose_pending_state goose_pending_execute_task(goose_pending_result pending_result) {
	if (!pending_result) {
		return GOOSE_PENDING_ERROR;
	}
	auto wrapper = reinterpret_cast<PendingStatementWrapper *>(pending_result);
	if (!wrapper->statement) {
		return GOOSE_PENDING_ERROR;
	}
	if (wrapper->statement->HasError()) {
		return GOOSE_PENDING_ERROR;
	}
	PendingExecutionResult return_value;
	try {
		return_value = wrapper->statement->ExecuteTask();
	} catch (std::exception &ex) {
		wrapper->statement->SetError(goose::ErrorData(ex));
		return GOOSE_PENDING_ERROR;
	}
	switch (return_value) {
	case PendingExecutionResult::EXECUTION_FINISHED:
	case PendingExecutionResult::RESULT_READY:
		return GOOSE_PENDING_RESULT_READY;
	case PendingExecutionResult::BLOCKED:
	case PendingExecutionResult::NO_TASKS_AVAILABLE:
		return GOOSE_PENDING_NO_TASKS_AVAILABLE;
	case PendingExecutionResult::RESULT_NOT_READY:
		return GOOSE_PENDING_RESULT_NOT_READY;
	default:
		return GOOSE_PENDING_ERROR;
	}
}

bool goose_pending_execution_is_finished(goose_pending_state pending_state) {
	switch (pending_state) {
	case GOOSE_PENDING_RESULT_READY:
		return PendingQueryResult::IsResultReady(PendingExecutionResult::RESULT_READY);
	case GOOSE_PENDING_NO_TASKS_AVAILABLE:
		return PendingQueryResult::IsResultReady(PendingExecutionResult::NO_TASKS_AVAILABLE);
	case GOOSE_PENDING_RESULT_NOT_READY:
		return PendingQueryResult::IsResultReady(PendingExecutionResult::RESULT_NOT_READY);
	case GOOSE_PENDING_ERROR:
		return PendingQueryResult::IsResultReady(PendingExecutionResult::EXECUTION_ERROR);
	default:
		return PendingQueryResult::IsResultReady(PendingExecutionResult::EXECUTION_ERROR);
	}
}

goose_state goose_execute_pending(goose_pending_result pending_result, goose_result *out_result) {
	if (!pending_result || !out_result) {
		return GooseError;
	}
	memset(out_result, 0, sizeof(goose_result));
	auto wrapper = reinterpret_cast<PendingStatementWrapper *>(pending_result);
	if (!wrapper->statement) {
		return GooseError;
	}

	goose::unique_ptr<goose::QueryResult> result;
	try {
		result = wrapper->statement->Execute();
	} catch (std::exception &ex) {
		goose::ErrorData error(ex);
		result = goose::make_uniq<goose::MaterializedQueryResult>(std::move(error));
	}

	wrapper->statement.reset();
	return GooseTranslateResult(std::move(result), out_result);
}
