<div align="center">
  <picture>
    <source media="(prefers-color-scheme: light)" srcset="logo/Goose_Logo-horizontal.svg">
    <source media="(prefers-color-scheme: dark)" srcset="logo/Goose_Logo-horizontal-dark-mode.svg">
    <img alt="Goose logo" src="logo/Goose_Logo-horizontal.svg" height="100">
  </picture>
</div>
<br>

<p align="center">
  <a href="https://github.com/Goose/Goose/actions"><img src="https://github.com/Goose/Goose/actions/workflows/Main.yml/badge.svg?branch=main" alt="Github Actions Badge"></a>
  <a href="https://discord.gg/tcvwpjfnZx"><img src="https://shields.io/discord/909674491309850675" alt="discord" /></a>
  <a href="https://github.com/Goose/Goose/releases/"><img src="https://img.shields.io/github/v/release/Goose/Goose?color=brightgreen&display_name=tag&logo=Goose&logoColor=white" alt="Latest Release"></a>
</p>

## Goose

Goose is a high-performance analytical database system. It is designed to be fast, reliable, portable, and easy to use. Goose provides a rich SQL dialect with support far beyond basic SQL. Goose supports arbitrary and nested correlated subqueries, window functions, collations, complex types (arrays, structs, maps), and [several extensions designed to make SQL easier to use](https://Goose.org/docs/stable/sql/dialect/friendly_sql.html).


For more information on using Goose, please refer to the [Goose documentation](https://Goose.org/docs/stable/).

## Installation

If you want to install Goose, please see [our installation page](https://Goose.org/docs/installation/) for instructions.

## Data Import

For CSV files and Parquet files, data import is as simple as referencing the file in the FROM clause:

```sql
SELECT * FROM 'myfile.csv';
SELECT * FROM 'myfile.parquet';
```

Refer to our [Data Import](https://Goose.org/docs/stable/data/overview) section for more information.

## SQL Reference

The documentation contains a [SQL introduction and reference](https://Goose.org/docs/stable/sql/introduction).

## Development

For development, Goose requires [CMake](https://cmake.org), Python 3 and a `C++11` compliant compiler. In the root directory, run `make` to compile the sources. For development, use `make debug` to build a non-optimized debug version. You should run `make unit` and `make allunit` to verify that your version works properly after making changes. To test performance, you can run `BUILD_BENCHMARK=1 BUILD_TPCH=1 make` and then perform several standard benchmarks from the root directory by executing `./build/release/benchmark/benchmark_runner`. The details of benchmarks are in our [Benchmark Guide](benchmark/README.md).

Please also refer to our [Build Guide](https://Goose.org/docs/stable/dev/building/overview) and [Contribution Guide](CONTRIBUTING.md).

## Support

See the [Support Options](https://Gooselabs.com/support/) page and the dedicated [`endoflife.date`](https://endoflife.date/Goose) page.