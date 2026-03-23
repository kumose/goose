#include <goose/parallel/interrupt.h>
#include <goose/execution/executor.h>
#include <goose/main/client_context.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <condition_variable>

namespace goose {

InterruptState::InterruptState() : mode(InterruptMode::NO_INTERRUPTS) {
}
InterruptState::InterruptState(weak_ptr<Task> task) : mode(InterruptMode::TASK), current_task(std::move(task)) {
}
InterruptState::InterruptState(weak_ptr<InterruptDoneSignalState> signal_state_p)
    : mode(InterruptMode::BLOCKING), signal_state(std::move(signal_state_p)) {
}

void InterruptState::Callback() const {
	if (mode == InterruptMode::TASK) {
		auto task = current_task.lock();

		if (!task) {
			return;
		}

		task->Reschedule();
	} else if (mode == InterruptMode::BLOCKING) {
		auto signal_state_l = signal_state.lock();

		if (!signal_state_l) {
			return;
		}

		// Signal the caller, who is currently blocked
		signal_state_l->Signal();
	} else {
		throw InternalException("Callback made on InterruptState without valid interrupt mode specified");
	}
}

void InterruptDoneSignalState::Signal() {
	{
		unique_lock<mutex> lck {lock};
		done = true;
	}
	cv.notify_all();
}

void InterruptDoneSignalState::Await() {
	std::unique_lock<std::mutex> lck(lock);
	cv.wait(lck, [&]() { return done; });

	// Reset after signal received
	done = false;
}

} // namespace goose
