add_library_unity(goose_func_ops_main OBJECT add.cc arithmetic.cc
                  multiply.cc subtract.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_ops_main>
    PARENT_SCOPE)
