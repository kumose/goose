#include "add_numbers.h"
#include "goose_extension.h"

GOOSE_EXTENSION_ENTRYPOINT(goose_connection connection, goose_extension_info info, goose_extension_access *access) {
	// Register a demo function
	RegisterAddNumbersFunction(connection);

#ifdef GOOSE_EXTENSION_API_VERSION_UNSTABLE
	// Test using the unstable API
	goose_arrow result;
	auto api_result = goose_query_arrow(connection, "SELECT 1 as a", &result);

	if (api_result != goose_state::GooseSuccess) {
		access->set_error(info, "Arrow Query failed during initialization");
		return false;
	}

	goose_destroy_arrow(&result);
#endif

	// Return true to indicate succesful initialization
	return true;
}
