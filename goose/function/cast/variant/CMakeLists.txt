add_library_unity(goose_variant_cast OBJECT from_variant.cc to_variant.cc
                  to_json.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_variant_cast>
    PARENT_SCOPE)
