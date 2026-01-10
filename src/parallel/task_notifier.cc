#include <goose/parallel/task_notifier.h>
#include <goose/main/client_context.h>
#include <goose/main/client_context_state.h>

namespace goose {

TaskNotifier::TaskNotifier(optional_ptr<ClientContext> context_p) : context(context_p) {
	if (context) {
		for (auto &state : context->registered_state->States()) {
			state->OnTaskStart(*context);
		}
	}
}

TaskNotifier::~TaskNotifier() {
	if (context) {
		for (auto &state : context->registered_state->States()) {
			state->OnTaskStop(*context);
		}
	}
}

} // namespace goose
