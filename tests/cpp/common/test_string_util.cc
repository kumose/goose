#include <goose/common/string_util.h>
#include <goose/common/types/value.h>
#include <goose/common/types-import.h>
#include <goose/common/exception/parser_exception.h>
#include <goose/testing/catch.h>

#include <goose/common/vector.h>
#include <string>
#include <cstring>

using namespace goose;

TEST_CASE("Test strcmp() to ensure platform sanity", "[comparison]") {
	int res;
	res = strcmp("ZZZ", "ZZZ");
	REQUIRE(res == 0);

	res = strcmp("ZZZ", "HXR");
	REQUIRE(res > 0);

	res = strcmp("ZZZ", "NUT");
	REQUIRE(res > 0);

	res = strcmp("HXR", "ZZZ");
	REQUIRE(res < 0);

	res = strcmp("HXR", "HXR");
	REQUIRE(res == 0);

	res = strcmp("HXR", "NUT");
	REQUIRE(res < 0);

	res = strcmp("NUT", "ZZZ");
	REQUIRE(res < 0);

	res = strcmp("NUT", "HXR");
	REQUIRE(res > 0);

	res = strcmp("NUT", "NUT");
	REQUIRE(res == 0);

	Value zzz("ZZZ");
	Value hxr("HXR");
	Value nut("NUT");

	REQUIRE_FALSE(zzz > zzz);
	REQUIRE(zzz > hxr);
	REQUIRE(zzz > nut);

	REQUIRE(zzz >= zzz);
	REQUIRE(zzz >= hxr);
	REQUIRE(zzz >= nut);

	REQUIRE(zzz <= zzz);
	REQUIRE_FALSE(zzz <= hxr);
	REQUIRE_FALSE(zzz <= nut);

	REQUIRE(zzz == zzz);
	REQUIRE_FALSE(zzz == hxr);
	REQUIRE_FALSE(zzz == nut);

	REQUIRE_FALSE(zzz != zzz);
	REQUIRE(zzz != hxr);
	REQUIRE(zzz != nut);

	REQUIRE_FALSE(hxr > zzz);
	REQUIRE_FALSE(hxr > hxr);
	REQUIRE_FALSE(hxr > nut);

	REQUIRE_FALSE(hxr >= zzz);
	REQUIRE(hxr >= hxr);
	REQUIRE_FALSE(hxr >= nut);

	REQUIRE(hxr <= zzz);
	REQUIRE(hxr <= hxr);
	REQUIRE(hxr <= nut);

	REQUIRE_FALSE(hxr == zzz);
	REQUIRE(hxr == hxr);
	REQUIRE_FALSE(hxr == nut);

	REQUIRE(hxr != zzz);
	REQUIRE_FALSE(hxr != hxr);
	REQUIRE(hxr != nut);

	REQUIRE_FALSE(nut > zzz);
	REQUIRE(nut > hxr);
	REQUIRE_FALSE(nut > nut);

	REQUIRE_FALSE(nut >= zzz);
	REQUIRE(nut >= hxr);
	REQUIRE(nut >= nut);

	REQUIRE(nut <= zzz);
	REQUIRE_FALSE(nut <= hxr);
	REQUIRE(nut <= nut);

	REQUIRE_FALSE(nut == zzz);
	REQUIRE_FALSE(nut == hxr);
	REQUIRE(nut == nut);

	REQUIRE(nut != zzz);
	REQUIRE(nut != hxr);
	REQUIRE_FALSE(nut != nut);
}

TEST_CASE("Test join vector items", "[string_util]") {
	SECTION("Three string items") {
		goose::vector<std::string> str_items = {"abc", "def", "ghi"};
		std::string result = StringUtil::Join(str_items, ",");
		REQUIRE(result == "abc,def,ghi");
	}

	SECTION("One string item") {
		goose::vector<std::string> str_items = {"abc"};
		std::string result = StringUtil::Join(str_items, ",");
		REQUIRE(result == "abc");
	}

	SECTION("No string items") {
		goose::vector<std::string> str_items;
		std::string result = StringUtil::Join(str_items, ",");
		REQUIRE(result == "");
	}

	SECTION("Three int items") {
		goose::vector<int> int_items = {1, 2, 3};
		std::string result =
		    StringUtil::Join(int_items, int_items.size(), ", ", [](const int &item) { return to_string(item); });
		REQUIRE(result == "1, 2, 3");
	}

	SECTION("One int item") {
		goose::vector<int> int_items = {1};
		std::string result =
		    StringUtil::Join(int_items, int_items.size(), ", ", [](const int &item) { return to_string(item); });
		REQUIRE(result == "1");
	}

	SECTION("No int items") {
		goose::vector<int> int_items;
		std::string result =
		    StringUtil::Join(int_items, int_items.size(), ", ", [](const int &item) { return to_string(item); });
		REQUIRE(result == "");
	}
}

TEST_CASE("Test SplitWithParentheses", "[string_util]") {
	SECTION("Standard split") {
		REQUIRE(StringUtil::SplitWithParentheses("") == goose::vector<string> {});
		REQUIRE(StringUtil::SplitWithParentheses("x") == goose::vector<string> {"x"});
		REQUIRE(StringUtil::SplitWithParentheses("hello") == goose::vector<string> {"hello"});
		REQUIRE(StringUtil::SplitWithParentheses("hello,world") == goose::vector<string> {"hello", "world"});
	}

	SECTION("Single item with parentheses") {
		REQUIRE(StringUtil::SplitWithParentheses("STRUCT(year BIGINT, month BIGINT, day BIGINT)") ==
		        goose::vector<string> {"STRUCT(year BIGINT, month BIGINT, day BIGINT)"});
		REQUIRE(StringUtil::SplitWithParentheses("(apple)") == goose::vector<string> {"(apple)"});
		REQUIRE(StringUtil::SplitWithParentheses("(apple, pear)") == goose::vector<string> {"(apple, pear)"});
		REQUIRE(StringUtil::SplitWithParentheses("(apple, pear) banana") ==
		        goose::vector<string> {"(apple, pear) banana"});
		REQUIRE(StringUtil::SplitWithParentheses("banana (apple, pear)") ==
		        goose::vector<string> {"banana (apple, pear)"});
		REQUIRE(StringUtil::SplitWithParentheses("banana (apple, pear) banana") ==
		        goose::vector<string> {"banana (apple, pear) banana"});
	}

	SECTION("Multiple items with parentheses") {
		REQUIRE(StringUtil::SplitWithParentheses("map::MAP(ANY,ANY),key::ANY") ==
		        goose::vector<string> {"map::MAP(ANY,ANY)", "key::ANY"});
		REQUIRE(StringUtil::SplitWithParentheses("extra,STRUCT(year BIGINT, month BIGINT, day BIGINT)") ==
		        goose::vector<string> {"extra", "STRUCT(year BIGINT, month BIGINT, day BIGINT)"});
		REQUIRE(StringUtil::SplitWithParentheses("extra,STRUCT(year BIGINT, month BIGINT, day BIGINT),extra") ==
		        goose::vector<string> {"extra", "STRUCT(year BIGINT, month BIGINT, day BIGINT)", "extra"});
		REQUIRE(StringUtil::SplitWithParentheses("aa(bb)cc,dd(ee)ff") ==
		        goose::vector<string> {"aa(bb)cc", "dd(ee)ff"});
		REQUIRE(StringUtil::SplitWithParentheses("aa(bb cc,dd),ee(f,,f)gg") ==
		        goose::vector<string> {"aa(bb cc,dd)", "ee(f,,f)gg"});
	}

	SECTION("Leading and trailing separators") {
		REQUIRE(StringUtil::SplitWithParentheses(",") == goose::vector<string> {""});
		REQUIRE(StringUtil::SplitWithParentheses(",,") == goose::vector<string> {"", ""});
		REQUIRE(StringUtil::SplitWithParentheses("aa,") == goose::vector<string> {"aa"});
		REQUIRE(StringUtil::SplitWithParentheses(",aa") == goose::vector<string> {"", "aa"});
		REQUIRE(StringUtil::SplitWithParentheses(",(aa,),") == goose::vector<string> {"", "(aa,)"});
	}

	SECTION("Leading and trailing spaces") {
		REQUIRE(StringUtil::SplitWithParentheses(" ") == goose::vector<string> {" "});
		REQUIRE(StringUtil::SplitWithParentheses("   ") == goose::vector<string> {"   "});
		REQUIRE(StringUtil::SplitWithParentheses("   , ") == goose::vector<string> {"   ", " "});
		REQUIRE(StringUtil::SplitWithParentheses("aa, bb") == goose::vector<string> {"aa", " bb"});
		REQUIRE(StringUtil::SplitWithParentheses(" aa,(bb, cc) ") == goose::vector<string> {" aa", "(bb, cc) "});
	}

	SECTION("Nested parentheses") {
		REQUIRE(StringUtil::SplitWithParentheses("STRUCT(aa BIGINT, bb STRUCT(cc BIGINT, dd BIGINT, BIGINT))") ==
		        goose::vector<string> {"STRUCT(aa BIGINT, bb STRUCT(cc BIGINT, dd BIGINT, BIGINT))"});
		REQUIRE(StringUtil::SplitWithParentheses("(((aa)))") == goose::vector<string> {"(((aa)))"});
		REQUIRE(StringUtil::SplitWithParentheses("((aa, bb))") == goose::vector<string> {"((aa, bb))"});
		REQUIRE(StringUtil::SplitWithParentheses("aa,(bb,(cc,dd)),ee") ==
		        goose::vector<string> {"aa", "(bb,(cc,dd))", "ee"});
	}

	SECTION("other parentheses") {
		REQUIRE(StringUtil::SplitWithParentheses(" aa,[bb, cc] )", ',', '[', ']') ==
		        goose::vector<string> {" aa", "[bb, cc] )"});
	}

	SECTION("other separators") {
		REQUIRE(StringUtil::SplitWithParentheses(" aa|(bb| cc),dd", '|') ==
		        goose::vector<string> {" aa", "(bb| cc),dd"});
	}

	SECTION("incongruent parentheses") {
		REQUIRE_THROWS(StringUtil::SplitWithParentheses("("));
		REQUIRE_THROWS(StringUtil::SplitWithParentheses(")"));
		REQUIRE_THROWS(StringUtil::SplitWithParentheses("aa(bb"));
		REQUIRE_THROWS(StringUtil::SplitWithParentheses("aa)bb"));
		REQUIRE_THROWS(StringUtil::SplitWithParentheses("(aa)bb)"));
		REQUIRE_THROWS(StringUtil::SplitWithParentheses("(aa(bb)"));
	}
}

TEST_CASE("Test split quoted strings", "[string_util]") {
	SECTION("Empty string") {
		REQUIRE(StringUtil::SplitWithQuote("") == goose::vector<string> {});
	}

	SECTION("Empty string with space") {
		REQUIRE(StringUtil::SplitWithQuote(" ") == goose::vector<string> {});
	}

	SECTION("One item") {
		REQUIRE(StringUtil::SplitWithQuote("x") == goose::vector<string> {"x"});
	}

	SECTION("One item with space") {
		REQUIRE(StringUtil::SplitWithQuote(" x ") == goose::vector<string> {"x"});
	}

	SECTION("One item with quote") {
		REQUIRE(StringUtil::SplitWithQuote("\"x\"") == goose::vector<string> {"x"});
	}

	SECTION("One empty item with quote") {
		REQUIRE(StringUtil::SplitWithQuote("\"\"") == goose::vector<string> {""});
	}

	SECTION("One empty item, followed by non-empty one - Or vise versa") {
		REQUIRE(StringUtil::SplitWithQuote("\"\",hello") == goose::vector<string> {"", "hello"});
		REQUIRE(StringUtil::SplitWithQuote(",\"hello\"") == goose::vector<string> {"", "hello"});
		REQUIRE(StringUtil::SplitWithQuote(",hello") == goose::vector<string> {"", "hello"});
		REQUIRE(StringUtil::SplitWithQuote("\"\",\"hello\"") == goose::vector<string> {"", "hello"});

		REQUIRE(StringUtil::SplitWithQuote("\"hello\",") == goose::vector<string> {"hello", ""});
		REQUIRE(StringUtil::SplitWithQuote("hello,\"\"") == goose::vector<string> {"hello", ""});
		REQUIRE(StringUtil::SplitWithQuote("hello,") == goose::vector<string> {"hello", ""});
		REQUIRE(StringUtil::SplitWithQuote("\"hello\",\"\"") == goose::vector<string> {"hello", ""});
	}

	SECTION("One quoted item with spaces") {
		REQUIRE(StringUtil::SplitWithQuote(" \" x y \" ") == goose::vector<string> {" x y "});
	}

	SECTION("One quoted item with a delimiter") {
		REQUIRE(StringUtil::SplitWithQuote("\"x,y\"") == goose::vector<string> {"x,y"});
	}

	SECTION("Three items") {
		REQUIRE(StringUtil::SplitWithQuote("x,y,z") == goose::vector<string> {"x", "y", "z"});
	}

	SECTION("Three items, with and without quote") {
		REQUIRE(StringUtil::SplitWithQuote("x,\"y\",z") == goose::vector<string> {"x", "y", "z"});
	}

	SECTION("Even more items, with and without quote") {
		REQUIRE(StringUtil::SplitWithQuote("a,b,c,d,e,f,g") ==
		        goose::vector<string> {"a", "b", "c", "d", "e", "f", "g"});
	}

	SECTION("Three empty items") {
		REQUIRE(StringUtil::SplitWithQuote(",,") == goose::vector<string> {"", "", ""});
	}

	SECTION("Three empty quoted items") {
		REQUIRE(StringUtil::SplitWithQuote("\"\",\"\",\"\"") == goose::vector<string> {"", "", ""});
	}

	SECTION("Unclosed quote") {
		REQUIRE_THROWS_AS(StringUtil::SplitWithQuote("\""), ParserException);
		REQUIRE_THROWS_AS(StringUtil::SplitWithQuote("\"x"), ParserException);
		REQUIRE_THROWS_AS(StringUtil::SplitWithQuote("\"x "), ParserException);
		REQUIRE_THROWS_AS(StringUtil::SplitWithQuote("\","), ParserException);
		REQUIRE_THROWS_AS(StringUtil::SplitWithQuote("\"x,"), ParserException);
	}

	SECTION("Unexpected quote") {
		REQUIRE_THROWS_AS(StringUtil::SplitWithQuote("abc\"def"), ParserException);
	}

	SECTION("Missing delimiter") {
		REQUIRE_THROWS_AS(StringUtil::SplitWithQuote("\"x\"\"y\""), ParserException);
		REQUIRE_THROWS_AS(StringUtil::SplitWithQuote("\"x\" \"y\""), ParserException);
		REQUIRE_THROWS_AS(StringUtil::SplitWithQuote("x y"), ParserException);
	}
}

TEST_CASE("Test path utilities", "[string_util]") {
	SECTION("File name") {
		REQUIRE("bin" == StringUtil::GetFileName("/usr/bin/"));
		REQUIRE("foo.txt" == StringUtil::GetFileName("foo.txt"));
		REQUIRE("foo.txt" == StringUtil::GetFileName("tmp/foo.txt"));
		REQUIRE("foo.txt" == StringUtil::GetFileName("tmp\\foo.txt"));
		REQUIRE("foo.txt" == StringUtil::GetFileName("/tmp/foo.txt"));
		REQUIRE("foo.txt" == StringUtil::GetFileName("\\tmp\\foo.txt"));
		REQUIRE("foo.txt" == StringUtil::GetFileName("foo.txt/."));
		REQUIRE("foo.txt" == StringUtil::GetFileName("foo.txt/./"));
		REQUIRE("foo.txt" == StringUtil::GetFileName("foo.txt/.//"));
		REQUIRE("foo.txt" == StringUtil::GetFileName("foo.txt\\."));
		REQUIRE("foo.txt" == StringUtil::GetFileName("foo.txt\\.\\"));
		REQUIRE("foo.txt" == StringUtil::GetFileName("foo.txt\\.\\\\"));
		REQUIRE(".." == StringUtil::GetFileName(".."));
		REQUIRE("" == StringUtil::GetFileName("/"));
	}

	SECTION("File extension") {
		REQUIRE("cc" == StringUtil::GetFileExtension("test.cc"));
		REQUIRE("gz" == StringUtil::GetFileExtension("test.cc.gz"));
		REQUIRE("" == StringUtil::GetFileExtension("test"));
		REQUIRE("" == StringUtil::GetFileExtension(".gitignore"));
	}

	SECTION("File stem (base name)") {
		REQUIRE("test" == StringUtil::GetFileStem("test.cc"));
		REQUIRE("test.cc" == StringUtil::GetFileStem("test.cc.gz"));
		REQUIRE("test" == StringUtil::GetFileStem("test"));
		REQUIRE(".gitignore" == StringUtil::GetFileStem(".gitignore"));

		REQUIRE("test" == StringUtil::GetFileStem("test.cc/"));
		REQUIRE("test" == StringUtil::GetFileStem("test.cc/."));
		REQUIRE("test" == StringUtil::GetFileStem("test.cc/./"));
		REQUIRE("test" == StringUtil::GetFileStem("test.cc/.//"));
		REQUIRE("test" == StringUtil::GetFileStem("test.cc\\"));
		REQUIRE("test" == StringUtil::GetFileStem("test.cc\\."));
		REQUIRE("test" == StringUtil::GetFileStem("test.cc\\.\\"));
		REQUIRE("test" == StringUtil::GetFileStem("test.cc\\.\\\\"));
		REQUIRE(".." == StringUtil::GetFileStem(".."));
		REQUIRE("" == StringUtil::GetFileStem("/"));
		REQUIRE("test" == StringUtil::GetFileStem("tmp/test.txt"));
		REQUIRE("test" == StringUtil::GetFileStem("tmp\\test.txt"));
		REQUIRE("test" == StringUtil::GetFileStem("/tmp/test.txt"));
		REQUIRE("test" == StringUtil::GetFileStem("\\tmp\\test.txt"));
	}

	SECTION("File path") {
		REQUIRE("/usr/local/bin" == StringUtil::GetFilePath("/usr/local/bin/test.cc"));
		REQUIRE("\\usr\\local\\bin" == StringUtil::GetFilePath("\\usr\\local\\bin\\test.cc"));
		REQUIRE("tmp" == StringUtil::GetFilePath("tmp/test.txt"));
		REQUIRE("tmp" == StringUtil::GetFilePath("tmp\\test.txt"));
		REQUIRE("/tmp" == StringUtil::GetFilePath("/tmp/test.txt"));
		REQUIRE("\\tmp" == StringUtil::GetFilePath("\\tmp\\test.txt"));
		REQUIRE("/tmp" == StringUtil::GetFilePath("/tmp/test.txt/"));
		REQUIRE("\\tmp" == StringUtil::GetFilePath("\\tmp\\test.txt\\"));
		REQUIRE("/tmp" == StringUtil::GetFilePath("/tmp//test.txt"));
		REQUIRE("\\tmp" == StringUtil::GetFilePath("\\tmp\\\\test.txt"));
	}
}

TEST_CASE("Test JSON Parsing", "[string_util]") {
	auto complex_json = StringUtil::ParseJSONMap(R"JSON_LITERAL(
	{
    "crs": {
        "$schema": "https://proj.org/schemas/v0.7/projjson.schema.json",
        "type": "GeographicCRS",
        "name": "WGS 84",
        "datum_ensemble": {
            "name": "World Geodetic System 1984 ensemble",
            "members": [
                {
                    "name": "World Geodetic System 1984 (Transit)",
                    "id": {
                        "authority": "EPSG",
                        "code": 1166
                    }
                },
                {
                    "name": "World Geodetic System 1984 (G730)",
                    "id": {
                        "authority": "EPSG",
                        "code": 1152
                    }
                },
                {
                    "name": "World Geodetic System 1984 (G873)",
                    "id": {
                        "authority": "EPSG",
                        "code": 1153
                    }
                },
                {
                    "name": "World Geodetic System 1984 (G1150)",
                    "id": {
                        "authority": "EPSG",
                        "code": 1154
                    }
                },
                {
                    "name": "World Geodetic System 1984 (G1674)",
                    "id": {
                        "authority": "EPSG",
                        "code": 1155
                    }
                },
                {
                    "name": "World Geodetic System 1984 (G1762)",
                    "id": {
                        "authority": "EPSG",
                        "code": 1156
                    }
                },
                {
                    "name": "World Geodetic System 1984 (G2139)",
                    "id": {
                        "authority": "EPSG",
                        "code": 1309
                    }
                },
                {
                    "name": "World Geodetic System 1984 (G2296)",
                    "id": {
                        "authority": "EPSG",
                        "code": 1383
                    }
                }
            ],
            "ellipsoid": {
                "name": "WGS 84",
                "semi_major_axis": 6378137,
                "inverse_flattening": 298.257223563
            },
            "accuracy": "2.0",
            "id": {
                "authority": "EPSG",
                "code": 6326
            }
        },
        "coordinate_system": {
            "subtype": "ellipsoidal",
            "axis": [
                {
                    "name": "Geodetic latitude",
                    "abbreviation": "Lat",
                    "direction": "north",
                    "unit": "degree"
                },
                {
                    "name": "Geodetic longitude",
                    "abbreviation": "Lon",
                    "direction": "east",
                    "unit": "degree"
                }
            ]
        },
        "scope": "Horizontal component of 3D system.",
        "area": "World.",
        "bbox": {
            "south_latitude": -90,
            "west_longitude": -180,
            "north_latitude": 90,
            "east_longitude": 180
        },
        "id": {
            "authority": "EPSG",
            "code": 4326
        }
    },
    "crs_type": "projjson"
}	)JSON_LITERAL");

	complex_json = StringUtil::ParseJSONMap(R"JSON_LITERAL(
	{
		"int": 42,
		"signed_int": -42,
		"real": 1.5,
		"null_val": null,
		"arr": [1, 2, 3],
		"obj": {
			"str_val": "val"
		},
		"empty_arr": [],
		"bool_t": true,
		"bool_f": false
	}
	)JSON_LITERAL");
}
