add_library_unity(
        goose_common_exception OBJECT binder_exception.cc catalog_exception.cc
  conversion_exception.cc parser_exception.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_common_exception>
    PARENT_SCOPE)
