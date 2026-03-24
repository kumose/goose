This is the SQL and C++ Goose Unittests

## Test Contract

Within SQL tests, the test runner (unittest) guarantees that these environment variables will be set. Many can be overridden when needed.

| Env Var          | Default Value              | Overrides                                                  | Notes                                              |
|------------------|----------------------------|-------------------------------------------------------------|---------------------------------------------------|
| `TEST_NAME` | e.g. `test/path/filename.test` | N/A | |
| `TEST_NAME_NO_SLASH` | e.g. `test_path_filename.test` | N/A | is always `TEST_NAME s@/@_@g`|
| `TEST_UUID` | random UUID (as string) | N/A | defined per invocation |
| `WORKING_DIR`    | e.g., `/Users/me/src/goose`     | `unittest --test-dir <path>`                                | default: wherever goose was sourced and built <br/> AKA: `__WORKING_DIRECTORY__`    |
| `BUILD_DIR`      | `{WORKING_DIR}/build/release` | N/A                                                         | read-only; derived from unittest path <br/> AKA: `__BUILD_DIRECTORY__`             |
| `DATA_DIR`  | `{WORKING_DIR}/data`        | `DATA_DIR=mydata unittest ...`<br/>also test configs, `test_env` specification | Should provide a copy of `goose/data/**`, use to test AWS, Azure, other VFS reads          |
| `TEMP_BASE` | `{WORKING_DIR}/goose_unittest_tempdir`          | via `unittest --test-temp-dir` (retains dir after test)      | use to test VFS writes <br/> AKA: `__TEST_DIR__`                            |
| `TEMP_DIR` | `{TEMP_BASE}/<process-id>`          | via `unittest --test-temp-dir` (retains dir after test)      | use to test VFS writes <br/> AKA: `__TEST_DIR__`                            |

Some tests require a particular environment variables to be set so they can properly run, usually this can be seen via `require-env VAR` in the test.

### Parallel CSV Reader

The tests located in `tests/cpp/parallel_csv/test_parallel_csv.cc` run the parallel CSV reader over multiple configurations of threads
and buffer sizes.

### ADBC

The ADBC tests are in `tests/cpp/api/adbc/test_adbc.cc`. To run them the Goose library path must be provided in the `GOOSE_INSTALL_LIB` variable.
