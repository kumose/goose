add_library_unity(goose_common_variant OBJECT variant.cc variant_value.cc
                  variant_value_convert.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_common_variant>
    PARENT_SCOPE)
