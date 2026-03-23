add_library_unity(goose_transformer_constraint OBJECT transform_constraint.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_transformer_constraint>
    PARENT_SCOPE)
