#include <goose/common/assert.h>
#include <goose/common/exception.h>

namespace goose {

void GooseAssertInternal(bool condition, const char *condition_name, const char *file, int linenr) {
#ifdef DISABLE_ASSERTIONS
	return;
#endif
	if (condition) {
		return;
	}
	throw InternalException("Assertion triggered in file \"%s\" on line %d: %s", file, linenr, condition_name);
}

} // namespace goose
