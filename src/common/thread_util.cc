#include <goose/common/thread.h>
#include <goose/common/types-import.h>

namespace goose {

void ThreadUtil::SleepMs(idx_t sleep_ms) {
#ifndef GOOSE_NO_THREADS
	std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
#else
	throw InvalidInputException("ThreadUtil::SleepMs requires Goose to be compiled with thread support");
#endif
}

} // namespace goose
