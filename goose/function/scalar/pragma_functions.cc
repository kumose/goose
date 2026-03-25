#include <goose/function/pragma/pragma_functions.h>

namespace goose {

void BuiltinFunctions::RegisterPragmaFunctions() {
	Register<PragmaQueries>();
	Register<PragmaFunctions>();
}

} // namespace goose
