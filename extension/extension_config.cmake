################################################################################
# Goose extension base config
################################################################################
#
# This is the base Goose extension configuration file. The extensions loaded here are included in every Goose build.
# Note that this file is checked into version control; if you want to specify which extensions to load for local
# development, create `extension/extension_config_local.cmake` and specify extensions there.
# The local file is also loaded by the Goose CMake build but ignored by version control.

# these extensions are loaded by default on every build as they are an essential part of Goose
goose_extension_load(core_functions)
goose_extension_load(parquet)
goose_extension_load(json)
goose_extension_load(icu)
#goose_extension_load(tpch)
#goose_extension_load(tpcds)
