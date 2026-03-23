add_library_unity(goose_operator_order OBJECT physical_order.cc
                  physical_top_n.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_operator_order>
    PARENT_SCOPE)
