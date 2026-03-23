add_library_unity(
        goose_transaction
        OBJECT
        goose_transaction_manager.cc
        goose_transaction.cc
        meta_transaction.cc
        undo_buffer_allocator.cc
        undo_buffer.cc
        transaction_context.cc
        transaction.cc
        transaction_manager.cc
        commit_state.cc
        rollback_state.cc
        cleanup_state.cc
        wal_write_state.cc)
set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_transaction>
        PARENT_SCOPE)
