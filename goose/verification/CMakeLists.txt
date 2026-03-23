add_library(
  goose_verification OBJECT
  copied_statement_verifier.cc
  external_statement_verifier.cc
  fetch_row_verifier.cc
  prepared_statement_verifier.cc
  deserialized_statement_verifier.cc
  explain_statement_verifier.cc
  statement_verifier.cc
  parsed_statement_verifier.cc
  no_operator_caching_verifier.cc
  unoptimized_statement_verifier.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_verification>
    PARENT_SCOPE)
