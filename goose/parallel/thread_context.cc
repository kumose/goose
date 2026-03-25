#include <goose/parallel/thread_context.h>
#include <goose/main/client_context.h>
#include <goose/logging/logger.h>
#include <goose/main/database.h>
#include <goose/logging/log_manager.h>

namespace goose {

ThreadContext::ThreadContext(ClientContext &context) : profiler(context) {
	LoggingContext log_context(LogContextScope::THREAD);

	log_context.connection_id = context.GetConnectionId();
	if (context.transaction.HasActiveTransaction()) {
		log_context.transaction_id = context.transaction.ActiveTransaction().global_transaction_id;
		auto query_id = context.transaction.GetActiveQuery();
		if (query_id == DConstants::INVALID_INDEX) {
			log_context.query_id = optional_idx();
		} else {
			log_context.query_id = query_id;
		}
	}

	log_context.thread_id = TaskScheduler::GetEstimatedCPUId();
	logger = LogManager::Get(context).CreateLogger(log_context, true);
}

ThreadContext::~ThreadContext() {
}

} // namespace goose
