add_library_unity(goose_progress_bar OBJECT progress_bar.cc
                  terminal_progress_bar_display.cc unscented_kalman_filter.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_progress_bar>
    PARENT_SCOPE)
