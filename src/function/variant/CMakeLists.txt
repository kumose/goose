add_library_unity(goose_function_variant OBJECT variant_shredding.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_function_variant>
    PARENT_SCOPE)
