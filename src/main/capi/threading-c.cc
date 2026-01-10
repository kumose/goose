#include <goose/main/capi/capi_internal.h>
#include <goose/parallel/task_scheduler.h>

using goose::DatabaseWrapper;

struct CAPITaskState {
	explicit CAPITaskState(goose::DatabaseInstance &db)
	    : db(db), marker(goose::make_uniq<goose::atomic<bool>>(true)), execute_count(0) {
	}

	goose::DatabaseInstance &db;
	goose::unique_ptr<goose::atomic<bool>> marker;
	goose::atomic<idx_t> execute_count;
};

void goose_execute_tasks(goose_database database, idx_t max_tasks) {
	if (!database) {
		return;
	}
	auto wrapper = reinterpret_cast<DatabaseWrapper *>(database);
	auto &scheduler = goose::TaskScheduler::GetScheduler(*wrapper->database->instance);
	scheduler.ExecuteTasks(max_tasks);
}

goose_task_state goose_create_task_state(goose_database database) {
	if (!database) {
		return nullptr;
	}
	auto wrapper = reinterpret_cast<DatabaseWrapper *>(database);
	auto state = new CAPITaskState(*wrapper->database->instance);
	return state;
}

void goose_execute_tasks_state(goose_task_state state_p) {
	if (!state_p) {
		return;
	}
	auto state = (CAPITaskState *)state_p;
	auto &scheduler = goose::TaskScheduler::GetScheduler(state->db);
	state->execute_count++;
	scheduler.ExecuteForever(state->marker.get());
}

idx_t goose_execute_n_tasks_state(goose_task_state state_p, idx_t max_tasks) {
	if (!state_p) {
		return 0;
	}
	auto state = (CAPITaskState *)state_p;
	auto &scheduler = goose::TaskScheduler::GetScheduler(state->db);
	return scheduler.ExecuteTasks(state->marker.get(), max_tasks);
}

void goose_finish_execution(goose_task_state state_p) {
	if (!state_p) {
		return;
	}
	auto state = (CAPITaskState *)state_p;
	*state->marker = false;
	if (state->execute_count > 0) {
		// signal to the threads to wake up
		auto &scheduler = goose::TaskScheduler::GetScheduler(state->db);
		scheduler.Signal(state->execute_count);
	}
}

bool goose_task_state_is_finished(goose_task_state state_p) {
	if (!state_p) {
		return false;
	}
	auto state = (CAPITaskState *)state_p;
	return !(*state->marker);
}

void goose_destroy_task_state(goose_task_state state_p) {
	if (!state_p) {
		return;
	}
	auto state = (CAPITaskState *)state_p;
	delete state;
}

bool goose_execution_is_finished(goose_connection con) {
	if (!con) {
		return false;
	}
	goose::Connection *conn = (goose::Connection *)con;
	return conn->context->ExecutionIsFinished();
}
