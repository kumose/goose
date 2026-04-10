# Goose 项目 README（强化叙事版）

# Goose（SQL IR 驱动的业务表达与执行系统）

[[English](README.md)]

Goose 不是一个传统意义上的分析型数据库。

它是一个基于 SQL IR（中间表示，Intermediate Representation）的**业务表达与执行系统**，用于在复杂场景下构建和组合能力，而不仅仅是处理数据。

在传统系统中：

* SQL 只用于查询数据
* 业务逻辑写在代码中
* 工作流通过服务编排实现
* AI 能力以孤立组件形式存在

系统的复杂度来自于：这些能力之间缺乏统一的表达方式。

Goose 的核心思路是：

> 使用 SQL 作为统一的中间表示，表达系统本身。

通过将 SQL 从“查询语言”提升为“表达语言”，Goose 可以统一描述：

* 工作流（workflow）
* 约束（constraints）
* 数据结构与转换
* 能力组合（capability composition）

从而使开发者可以以声明式方式构建系统，通过组合可复用的能力模块，而不是在命令式代码中进行大量手工拼接。

---

## 为什么是 SQL IR

传统架构缺少一层关键能力：

👉 **业务表达层（Business Expression Layer）**

这导致：

* 数据处理（SQL）
* 业务逻辑（代码）
* AI 流程（Pipeline / DAG）

被割裂在不同系统中。

Goose 将 SQL 作为统一的中间表示（SQL IR），使其成为：

> 一个可以承载业务逻辑、数据处理和能力编排的通用表达层

这不仅仅是语法扩展，而是执行模型的重构。

---

## 核心能力

Goose 在表达能力上的增强，使其可以承载复杂系统构建：

* 支持任意嵌套相关子查询（correlated subqueries）
* 窗口函数（window functions）
* 排序规则（collations）
* 复杂数据类型（arrays / structs / maps）

同时提供丰富的扩展机制，用于集成不同能力模块：

* 向量检索（vss）
* NLP 处理（gnef）
* HTTP 文件系统（httpfs）
* 数据集测试（TPC-H / TPC-DS）

---

## 能力组合（Capability Composition）

在 Goose 中：

* 每一个扩展模块都是一个“能力单元”
* SQL 负责描述这些能力如何组合与执行

这使得系统构建从：

👉 imperative wiring（命令式拼接）

转变为：

👉 declarative composition（声明式组合）

---

## 示例：NLP 工作流（gnef）

[gnef](https://github.com/kumose/gnef) 是一个基于 Goose 构建的 NLP 处理扩展，用于工业场景。

它展示了：

* 如何将 NLP 能力直接嵌入 SQL
* 如何通过 SQL 表达复杂处理流程
* 如何在统一表达层中组合多种能力

这正是 Goose 的核心价值体现：

👉 **能力不再分散在系统中，而是在 SQL IR 中被统一表达与组合**

---

## 客户端与使用方式

Goose 提供：

* 独立 CLI 客户端
* Python 客户端（与 pandas 等数据处理库深度集成）

详细使用方式请参考：
[https://pub.kumose.cc/goose](https://pub.kumose.cc/goose)

---

## 核心定义

> SQL 不再只是查询语言
> 而是系统的表达语言

> Goose 不是数据库的替代品
> 而是一层新的系统抽象

> 将 SQL 从查询语言转变为通用的业务表达层

---

## 在线示例

* [goose-demo](https://github.com/kumose/goose-demo)：扩展开发与应用示例

* [gnef](https://github.com/kumose/gnef)：NLP 工作流扩展


---

## 🛠️ 编译与构建

本项目使用 [kmpkg](https://github.com/kumose/kmcmake) 进行依赖管理与构建集成，可自动完成第三方库下载、依赖解析及编译参数配置，无需手动维护复杂的 CMake 配置。

### 0. 环境准备

* 操作系统：Linux（推荐 Ubuntu 20.04+ 或 CentOS 7+）

* 构建工具：CMake ≥ 3.31

* 编译器：GCC ≥ 9.4 或 Clang ≥ 12

* 依赖工具：kmpkg（必须正确安装，参考相关安装文档）

### 快速安装 kmpkg

```shell
git clone https://github.com/kumose/kmpkgcore.git
cd kmpkgcore
./bootstrap-kmpkg.sh
```

配置环境变量（可写入 ~/.bashrc 以永久生效，执行 source ~/.bashrc 生效）：

```shell
export KMPKG_HOME=/home/jeff/kmpkgcore
export PATH=$PATH:$KMPKG_HOME
export KMPKG_CMAKE=$KMPKG_HOME/scripts/buildsystems/kmpkg.cmake
```

---

### 1. 项目配置（可选）

* 完整依赖信息请参考项目根目录下的 `kmpkg.json` 文件

* 如需更新依赖基线版本，可修改 `kmpkg-configuration.json` 中 `default-registry` 下的 `baseline` 字段（baseline 可通过 git log 获取）

---

### 2. 项目编译

在项目根目录执行以下命令完成编译：

```bash
cmake --preset=default
cmake --build build -j$(nproc)
```

> * 若需运行 benchmark，添加 `-DKMCMAKE_BUILD_BENCHMARK=ON`
> * 若需编译示例，添加 `-DKMCMAKE_BUILD_EXAMPLES=ON`
> * `KMCMAKE_BUILD_TEST` 默认开启，安装时可使用 `-DKMCMAKE_BUILD_TEST=OFF`
> * 运行 Python 示例：`pip install goose-ir` 后执行示例代码

---

### 手动依赖管理（可选）

如需手动管理依赖，可使用标准 CMake：

```shell
mkdir build
cd build
cmake ..
make -j$(nproc)
```

注意：使用 `--preset=default` 需确保项目根目录存在对应的 CMake preset；手动管理依赖时需确保 `find_package` 能正确找到所有依赖库。

---

### 3. 编译产物说明

* goose：命令行可执行程序

* libgoose_shell.a：Shell 相关静态库

* libgoose.a：主静态库

* libgoose_benchmark.a：扩展开发用 Benchmark 库

* libgoose_testing.a：扩展开发用测试库

* extensions（扩展模块，部分自动集成）：

  * core_functions、icu、parquet、json：自动集成

  * autocomplete：命令行自动补全工具

  * httpfs：HTTP 文件系统扩展

  * tpcds、tpch：数据库测试扩展

  * vss：向量检索扩展

  * [gnef](https://github.com/kumose/gnef)：NLP 处理扩展

---

### 4. 运行测试（可选）

在项目根目录执行：

```shell
ctest --test-dir build
```

---

## 使用指南

### 1. 项目集成（CMake）

在你的 CMake 项目中集成 Goose 示例：

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

注意：扩展模块不应直接链接 `goose::goose_static`，而应在最终应用链接时统一关联；链接顺序上，扩展模块应在 `goose::goose_static` 之前。

---

### 2. 扩展开发

扩展开发示例 CMake 配置：

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

---

### 3. 应用初始化

应用中初始化 Goose 及扩展示例：

```cpp
#include <goose/shell/shell.h>
#include <goose/goose.h>
#include  <goose/extension/autocomplete/autocomplete_extension.h>
#include  <goose/extension/httpfs/httpfs_extension.h>
#include  <goose/extension/vss/vss_extension.h>

namespace goose {

    /// 用户需要实现该函数以初始化扩展
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

---

### 4. 应用编译

应用编译示例：

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

---

## LICENSE

本项目主要采用 **Apache License 2.0**，详细条款请参见项目根目录下的 LICENSE 文件。

本项目集成了以下第三方开源库，其各自仍遵循原有许可协议：

* duckdb: [https://github.com/duckdb/duckdb（MIT](https://github.com/duckdb/duckdb（MIT) License）

* pdqsort: [https://github.com/orlp/pdqsort（zlib/libpng](https://github.com/orlp/pdqsort（zlib/libpng) License）

* ska_sort: [https://github.com/skarupke/ska_sort（Boost](https://github.com/skarupke/ska_sort（Boost) Software License）

* vergesort: [https://github.com/Morwenn/vergesort（MIT](https://github.com/Morwenn/vergesort（MIT) License）

请注意：Apache 2.0 License 仅适用于本项目（kumo 项目）中的集成工作及原创代码，不适用于上述第三方库本身。
