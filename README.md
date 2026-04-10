# Goose Project README

# Goose (High-Performance Analytical Database System)

[[中文版](README_CN.md)]

Goose is a high-performance execution and expression system built on
an extensible SQL IR (Intermediate Representation). It enables developers
to construct complex systems by composing reusable capability modules,
rather than manually wiring them together in imperative code.

Rather than focusing solely on data processing like traditional analytical
databases, Goose uses SQL as a unified intermediate representation to
express workflows, constraints, and capability composition — turning SQL
from a query language into a general-purpose business expression layer.

Goose also supports advanced SQL features such as arbitrary nested
correlated subqueries, window functions, collations, and complex data
types (arrays, structs, maps), along with a rich set of extensions to
reduce the barrier to adoption.

Goose provides an independent CLI client and a Python client (deeply
integrated with data processing libraries such as pandas). For more
details, see the [Goose Official Documentation](https://pub.kumose.cc/goose).

**It is particularly important to emphasize that the core value of Goose lies in its SQL IR foundation.**  
It builds an extensible architecture with SQL as the unified intermediate
representation, with the core goal of strengthening business logic
expression and reducing development costs in complex scenarios.

This design is reflected in extension projects such as
[gnef](https://github.com/kumose/gnef), which integrates NLP workflows
directly into SQL, demonstrating how business expression and underlying
capabilities can be seamlessly composed.

> Turning SQL from a query language into a general-purpose business expression layer.

## Online Examples

- [goose-demo](https://github.com/kumose/goose-demo): Demonstrates the extension development process and practical application scenarios, helping you quickly get started with Goose extension development.

- [gnef](https://github.com/kumose/gnef): An NLP processing workflow example for industrial scenarios, a professional extension built on Goose.

## 🛠️ Compilation and Construction

This project uses [kmpkg](https://github.com/kumose/kmcmake) for dependency management and build integration, which can automatically complete the download of third-party libraries, dependency resolution, and compiler parameter configuration, eliminating the need to manually maintain complex CMake configurations.

### 0. Environment Preparation

- Operating System: Linux (Ubuntu 20.04+ or CentOS 7+ recommended)

- Build Tool: CMake ≥ 3.31

- Compiler: GCC ≥ 9.4 or Clang ≥ 12

- Dependency Tool: kmpkg (must be installed correctly, refer to relevant installation documents)

### Quick Installation of kmpkg

```shell
git clone https://github.com/kumose/kmpkgcore.git
cd kmpkgcore
./bootstrap-kmpkg.sh
```

Configure environment variables (can be written to ~/.bashrc for permanent effect; run source ~/.bashrc to take effect):

```shell
export KMPKG_HOME=/home/jeff/kmpkgcore
export PATH=$PATH:$KMPKG_HOME
export KMPKG_CMAKE=$KMPKG_HOME/scripts/buildsystems/kmpkg.cmake
```

### 1. Project Configuration (Optional)

- For complete dependency information, refer to the `kmpkg.json` file in the project root directory.

- To update the dependency baseline version, modify the `baseline` field under `default-registry` in the `kmpkg-configuration.json` file (the baseline can be obtained via the git log command).

### 2. Project Compilation

Execute the following commands in the project root directory to complete the compilation and construction:

```bash
cmake --preset=default
cmake --build build -j$(nproc)
```
> - if you want run benchmark, add `-DKMCMAKE_BUILD_BENCHMARK=ON`
> - if you want run example, add `-DKMCMAKE_BUILD_EXAMPLES=ON`
> - the `KMCMAKE_BUILD_TEST` is default on. when install it just `-DKMCMAKE_BUILD_TEST=OFF`
> - run python demo `pip install goose-ir` and then run the python demo.

#### Manual Dependency Management (Optional)

If you need to manage dependencies manually, you can compile using standard CMake commands:

```shell
mkdir build
cd build
cmake ..
make -j$(nproc)
```

Note: --preset=default requires that the corresponding CMake preset is defined in the project root directory; when managing dependencies manually, ensure that CMake's find_package can locate all required libraries.

### 3. Description of Compilation Products

- goose: Command-line executable program

- libgoose_shell.a: Shell-related static library

- libgoose.a: Main static library

- libgoose_benchmark.a: Benchmark library for extension development

- libgoose_testing.a: Testing library for extension development

- extensions (extension modules, some are automatically integrated):


    - core_functions, icu, parquet, json: Automatically integrated

    - autocomplete: Command-line auto-completion tool

    - httpfs: HTTP file system extension

    - tpcds, tpch: Database test extensions

    - vss: Vector search extension

    - [gnef](https://github.com/kumose/gnef): NLP processing extension

### 4. Run Tests (Optional)

Execute the following command in the project root directory to run test cases and verify the compilation results:

```shell
ctest --test-dir build
```

## Usage Guide

### 1. Project Integration (CMake)

It is best to use `kmpgk` manage de the goose:


#### Quick Start project (via kmpkg)

```shell
kmdo kmpkg gencmake -o . -n your-app-name
kmpkg new --application
kmpkg add port goose
```

To integrate Goose into your CMake project, the sample configuration is as follows:

```cmake
find_package(goose CONFIG REQUIRED)
target_link_libraries(main PRIVATE goose::goose_static)
target_link_libraries(main PRIVATE goose::goose_https)
target_link_libraries(main PRIVATE goose::goose_vss)
target_link_libraries(main PRIVATE goose::goose_tpch)
target_link_libraries(main PRIVATE goose::goose_tpcds)
target_link_libraries(main PRIVATE goose::goose_testing_main_static
        goose::goose_testing_static)
```

Note: Extension modules should not directly link to goose::goose_static; they should be associated when finally linking the application. When linking using MakeFile or CMake, extension modules should be linked before goose::goose_static.

### 2. Extension Development

Sample CMake configuration for extension development:

```cmake
file(GLOB_RECURSE JSON_SRC "*.cc")

goose_cc_extension(
        NAMESPACE ${PROJECT_NAME}
        NAME json
        SOURCES
        ${JSON_SRC}
        ABI_TYPE
        "CPP"
        INCLUDES
        include
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        PLINKS
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)
```

### 3. Application Configuration

Sample code for initializing Goose and extensions in an application:

```cpp
#include <goose/shell/shell.h>
#include <goose/goose.h>
#include  <goose/extension/autocomplete/autocomplete_extension.h>
#include  <goose/extension/httpfs/httpfs_extension.h>
#include  <goose/extension/vss/vss_extension.h>

namespace goose {

    /// Users need to implement this function to initialize extensions
    void init_extensions() {
        goose::UserConfig::instance().alter_string = "KSQL";
        auto rs = enable_extension_autoload("autocomplete", [](Goose &db) {
                db.LoadStaticExtension<AutocompleteExtension>();
                return ExtensionLoadResult::LOADED_EXTENSION;
            });
        TURBO_UNUSED(rs);
        rs = enable_extension_autoload("httpfs", [](Goose &db) {
                db.LoadStaticExtension<HttpfsExtension>();
                return ExtensionLoadResult::LOADED_EXTENSION;
            });
        TURBO_UNUSED(rs);
        rs = enable_extension_autoload("vss", [](Goose &db) {
                db.LoadStaticExtension<VssExtension>();
                return ExtensionLoadResult::LOADED_EXTENSION;
            });
        TURBO_UNUSED(rs);
    }
} // namespace goose
```

### 4. Application Compilation

Sample CMake configuration for application compilation:

```cmake
kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME ksql
        SOURCES
        shell/shell.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        goose::goose_shell_static
        goose::goose_static
        goose::goose_autocomplete
        goose::goose_vss
        goose::goose_httpfs
        ${KMCMAKE_DEPS_LINK}
        ${HTTPFS_DEPS_LINK}
        LINKOPTS
        ${KMCMAKE_STATIC_BIN_OPTION}
        PUBLIC
)
```

## LICENSE

The main license of this project is **Apache License 2.0**; for detailed terms, refer to the LICENSE file in the project root directory.

This project integrates code from the following third-party open-source libraries, and the relevant libraries still follow their original license agreements:

- duckdb: [https://github.com/duckdb/duckdb](https://github.com/duckdb/duckdb), licensed under the MIT License, see licenses/duckdb.license for details.

- pdqsort: [https://github.com/orlp/pdqsort](https://github.com/orlp/pdqsort), licensed under the zlib/libpng License, see licenses/pdq.license for details.

- ska_sort: [https://github.com/skarupke/ska_sort](https://github.com/skarupke/ska_sort), licensed under the Boost Software License, see licenses/ska.license for details.

- vergesort: [https://github.com/Morwenn/vergesort](https://github.com/Morwenn/vergesort), licensed under the MIT License, see licenses/vergesort.license for details.

Please note: The Apache 2.0 License only applies to the integration work and original code developed as part of this project (kumo project), not to the above-mentioned third-party open-source libraries themselves.
