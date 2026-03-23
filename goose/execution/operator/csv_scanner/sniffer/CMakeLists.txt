add_library_unity(
  goose_csv_sniffer
  OBJECT
  csv_sniffer.cc
  dialect_detection.cc
  header_detection.cc
  set_columns.cc
  type_detection.cc
  type_refinement.cc
  type_replacement.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_csv_sniffer>
    PARENT_SCOPE)
