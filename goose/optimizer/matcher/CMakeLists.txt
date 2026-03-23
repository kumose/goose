add_library_unity(goose_optimizer_matcher OBJECT expression_matcher.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_optimizer_matcher>
    PARENT_SCOPE)
