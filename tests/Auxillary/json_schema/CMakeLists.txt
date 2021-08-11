add_executable(aux_schema_validation_test schemaValidationTest.cpp)
target_link_libraries(aux_schema_validation_test PUBLIC aux_schema_validation nlohmann_json)
target_link_libraries(aux_schema_validation_test PUBLIC gtest gtest_main gmock)


add_test(
  NAME aux_schema_validation
  COMMAND aux_schema_validation_test
  )

add_executable(aux_schema_creation_test schemaCreationTest.cpp)
target_link_libraries(aux_schema_creation_test PUBLIC aux_make_schema aux_schema_validation nlohmann_json)
target_link_libraries(aux_schema_creation_test PUBLIC gtest gtest_main gmock)


add_test(
  NAME aux_schema_creation_test
  COMMAND aux_schema_creation_test
)

add_executable(aux_schema_key_insertion_test schemaKeyInsertionTest.cpp)
target_link_libraries(aux_schema_key_insertion_test PUBLIC aux_schema_key_insertion nlohmann_json)
target_link_libraries(aux_schema_key_insertion_test PUBLIC gtest gtest_main gmock test_helpers)


add_test(
  NAME aux_schema_key_insertion
  COMMAND aux_schema_key_insertion_test
  )