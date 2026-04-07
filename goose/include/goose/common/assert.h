//===----------------------------------------------------------------------===//
//                         Goose
//
// goose/common/assert.hpp
//
//
//===----------------------------------------------------------------------===//


#pragma once

#include <goose/common/winapi.h>
// clang-format off
#if ( \
    /* Not a debug build */ \
    !defined(DEBUG) && \
    /* FORCE_ASSERT is not set (enables assertions even on release mode when set to true) */ \
    !defined(GOOSE_FORCE_ASSERT) && \
    /* The project is not compiled for Microsoft Visual Studio */ \
    !defined(__MVS__) \
)
// clang-format on

//! On most builds, NDEBUG is defined, turning the assert call into a NO-OP
//! Only the 'else' condition is supposed to check the assertions
#include <assert.h>
#define D_ASSERT assert

namespace goose {
    GOOSE_API void GooseAssertInternal(bool condition, const char *condition_name, const char *file, int linenr);
}

#else
namespace goose {
GOOSE_API void GooseAssertInternal(bool condition, const char *condition_name, const char *file, int linenr);
}

#define D_ASSERT(condition) goose::GooseAssertInternal(bool(condition), #condition, __FILE__, __LINE__)
#define D_ASSERT_IS_ENABLED

#endif

//! Force assertion implementation, which always asserts whatever build type is used.
#define ALWAYS_ASSERT(condition) goose::GooseAssertInternal(bool(condition), #condition, __FILE__, __LINE__)
