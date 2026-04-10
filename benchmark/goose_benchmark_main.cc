
#include <benchmark/config.h>
#include <goose/common/file_system.h>
#include <goose/common/string_util.h>
#include <goose/extension/vss/vss_extension.h>
#include <goose/extension/tpch/tpch_extension.h>
#include <goose/extension/tpcds/tpcds_extension.h>
#include <goose/benchmark/goose_benchmark.h>

namespace goose {

    void init_goose_benchmark() {
        goose::set_benchmark_directory(testing::benchmark_root());
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
