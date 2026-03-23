add_library_unity(goose_func_variant_main OBJECT variant_utils.cc
                  variant_extract.cc variant_typeof.cc variant_normalize.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_variant_main>
    PARENT_SCOPE)
