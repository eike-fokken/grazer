add_executable(aux_schema_validation_test validationTest.cpp)
target_link_libraries(aux_schema_validation_test PUBLIC aux_schema_validation nlohmann_json)
target_link_libraries(aux_schema_validation_test PUBLIC gtest gtest_main gmock)


add_test(
  NAME aux_schema_validation
  COMMAND aux_schema_validation_test
  )
