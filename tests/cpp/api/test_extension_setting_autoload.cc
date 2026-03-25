#include <goose/testing/catch.h>
#include <goose/testing/test_helpers.h>

#include <iostream>
#include <map>
#include <set>

using namespace goose;
using namespace std;

TEST_CASE("Test autoload of extension settings", "[api]") {
	DBConfig config;
	config.SetOptionByName("timezone", "America/Los_Angeles");

	config.options.allow_unsigned_extensions = true;
	config.options.autoload_known_extensions = true;
	auto env_var = std::getenv("LOCAL_EXTENSION_REPO");
	if (!env_var) {
		return;
	}
	config.options.autoinstall_extension_repo = std::string(env_var);
	REQUIRE(config.options.unrecognized_options.count("timezone"));

	// Create a connection
	goose::unique_ptr<Goose> db;
	REQUIRE_NOTHROW(db = make_uniq<Goose>(nullptr, &config));
	Connection con(*db);

	auto res = con.Query("select current_setting('timezone')");
	REQUIRE(CHECK_COLUMN(res, 0, {Value("America/Los_Angeles")}));
}
