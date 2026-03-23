add_library_unity(goose_storage_table_variant OBJECT variant_shredding.cc
                  variant_unshredding.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_storage_table_variant>
    PARENT_SCOPE)
