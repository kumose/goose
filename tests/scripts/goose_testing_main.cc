#define CATCH_CONFIG_RUNNER
#include <goose/testing/catch.h>
#include <tests/config.h>
#include <goose/common/file_system.h>
#include <goose/common/string_util.h>
#include <goose/testing/sqlite/sqllogic_test_logger.h>
#include <goose/testing/test_helpers.h>
#include <goose/testing/test_config.h>
#include <goose/testing/scripts_runner.h>
#include <goose/testing/goose_testing_main.h>
#include <goose/extension/vss/vss_extension.h>
#include <goose/extension/tpch/tpch_extension.h>
#include <goose/extension/tpcds/tpcds_extension.h>

namespace goose {
    std::vector<std::string> enable_verification_excludes = {
        // too slow for verification
        "test/select5.test",
        "test/index",
        // optimization masks int32 overflow
        "test/random/aggregates/slt_good_102.test",
        "test/random/aggregates/slt_good_11.test",
        "test/random/aggregates/slt_good_115.test",
        "test/random/aggregates/slt_good_116.test",
        "test/random/aggregates/slt_good_118.test",
        "test/random/aggregates/slt_good_119.test",
        "test/random/aggregates/slt_good_122.test",
        "test/random/aggregates/slt_good_17.test",
        "test/random/aggregates/slt_good_20.test",
        "test/random/aggregates/slt_good_23.test",
        "test/random/aggregates/slt_good_25.test",
        "test/random/aggregates/slt_good_3.test",
        "test/random/aggregates/slt_good_30.test",
        "test/random/aggregates/slt_good_31.test",
        "test/random/aggregates/slt_good_38.test",
        "test/random/aggregates/slt_good_39.test",
        "test/random/aggregates/slt_good_4.test",
        "test/random/aggregates/slt_good_43.test",
        "test/random/aggregates/slt_good_46.test",
        "test/random/aggregates/slt_good_51.test",
        "test/random/aggregates/slt_good_56.test",
        "test/random/aggregates/slt_good_66.test",
        "test/random/aggregates/slt_good_7.test",
        "test/random/aggregates/slt_good_72.test",
        "test/random/aggregates/slt_good_82.test",
        "test/random/aggregates/slt_good_84.test",
        "test/random/aggregates/slt_good_85.test",
        "test/random/aggregates/slt_good_91.test",
        "test/random/expr/slt_good_15.test",
        "test/random/expr/slt_good_66.test",
        "test/random/expr/slt_good_91.test",
    };

    std::vector<std::string> excludes = {
        // tested separately
        "test/select1.test", "test/select2.test", "test/select3.test", "test/select4.test",
        // feature not supported
        "evidence/slt_lang_replace.test", // INSERT OR REPLACE
        "evidence/slt_lang_reindex.test", // REINDEX
        "evidence/slt_lang_update.test", // Multiple assignments to same column "x" in update
        "evidence/slt_lang_createtrigger.test", // TRIGGER
        "evidence/slt_lang_droptrigger.test", // TRIGGER
        // no + for varchar columns
        "test/index/random/10/slt_good_14.test", "test/index/random/10/slt_good_1.test",
        "test/index/random/10/slt_good_0.test", "test/index/random/10/slt_good_12.test",
        "test/index/random/10/slt_good_6.test", "test/index/random/10/slt_good_13.test",
        "test/index/random/10/slt_good_5.test", "test/index/random/10/slt_good_10.test",
        "test/index/random/10/slt_good_11.test", "test/index/random/10/slt_good_4.test",
        "test/index/random/10/slt_good_8.test", "test/index/random/10/slt_good_3.test",
        "test/index/random/10/slt_good_2.test", "test/index/random/100/slt_good_1.test",
        "test/index/random/100/slt_good_0.test", "test/index/random/1000/slt_good_0.test",
        "test/index/random/1000/slt_good_7.test", "test/index/random/1000/slt_good_6.test",
        "test/index/random/1000/slt_good_5.test", "test/index/random/1000/slt_good_8.test",
        // overflow in 32-bit integer multiplication (sqlite does automatic upcasting)
        "test/random/aggregates/slt_good_96.test", "test/random/aggregates/slt_good_75.test",
        "test/random/aggregates/slt_good_64.test", "test/random/aggregates/slt_good_9.test",
        "test/random/aggregates/slt_good_110.test", "test/random/aggregates/slt_good_101.test",
        "test/random/expr/slt_good_55.test", "test/random/expr/slt_good_115.test",
        "test/random/expr/slt_good_103.test",
        "test/random/expr/slt_good_80.test", "test/random/expr/slt_good_75.test",
        "test/random/expr/slt_good_42.test",
        "test/random/expr/slt_good_49.test", "test/random/expr/slt_good_24.test",
        "test/random/expr/slt_good_30.test",
        "test/random/expr/slt_good_8.test", "test/random/expr/slt_good_61.test",
        // dependencies between tables/views prevent dropping in Goose without CASCADE
        "test/index/view/1000/slt_good_0.test", "test/index/view/100/slt_good_0.test",
        "test/index/view/100/slt_good_5.test", "test/index/view/100/slt_good_1.test",
        "test/index/view/100/slt_good_3.test", "test/index/view/100/slt_good_4.test",
        "test/index/view/100/slt_good_2.test", "test/index/view/10000/slt_good_0.test",
        "test/index/view/10/slt_good_5.test", "test/index/view/10/slt_good_7.test",
        "test/index/view/10/slt_good_1.test", "test/index/view/10/slt_good_3.test",
        "test/index/view/10/slt_good_4.test", "test/index/view/10/slt_good_6.test",
        "test/index/view/10/slt_good_2.test",
        // strange error in hash comparison, results appear correct...
        "test/index/random/10/slt_good_7.test", "test/index/random/10/slt_good_9.test"
    };

    void init_goose_testing() {
        TestingDir::instance().test_directory = testing::goose_root_directory();
        goose::set_excludes_scripts(excludes);
        goose::set_verification_scripts(enable_verification_excludes);
        goose::set_test_directory({"tests/scripts"});
        auto rs = enable_extension_autoload("vss", [](Goose &db) {
                db.LoadStaticExtension<VssExtension>();
                return ExtensionLoadResult::LOADED_EXTENSION;
            });
        TURBO_UNUSED(rs);
        rs = enable_extension_autoload("tpch", [](Goose &db) {
                db.LoadStaticExtension<TpchExtension>();
                return ExtensionLoadResult::LOADED_EXTENSION;
            });
        TURBO_UNUSED(rs);
        rs = enable_extension_autoload("tpcds", [](Goose &db) {
                db.LoadStaticExtension<TpcdsExtension>();
                return ExtensionLoadResult::LOADED_EXTENSION;
            });
        TURBO_UNUSED(rs);
    }
} // namespace goose
