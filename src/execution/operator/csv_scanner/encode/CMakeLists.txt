add_library_unity(goose_csv_encode OBJECT csv_encoder.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_csv_encode>
    PARENT_SCOPE)
