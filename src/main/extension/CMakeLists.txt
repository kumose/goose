add_library(
  goose_main_extension OBJECT
  extension_alias.cc extension_helper.cc extension_install.cc
  extension_load.cc extension_loader.cc)

# Escape ; in the string to ensure this is treated as a plain string instead of
# a list
string(REPLACE ";" "\;" EXT_DIR_ESCAPED "${EXTENSION_DIRECTORIES}")

target_compile_definitions(
  goose_main_extension
  PRIVATE "GOOSE_EXTENSION_DIRECTORIES=\"${EXT_DIR_ESCAPED}\"")

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_main_extension>
    PARENT_SCOPE)
