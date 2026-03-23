add_library_unity(goose_csv_util OBJECT csv_error.cc csv_reader_options.cc
                  csv_validator.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_csv_util>
    PARENT_SCOPE)
