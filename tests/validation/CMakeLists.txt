add_executable(json_validation_test validationTest.cpp)
target_link_libraries(json_validation_test PUBLIC json_validation nlohmann_json)
target_link_libraries(json_validation_test PUBLIC gtest gtest_main gmock)


add_test(
  NAME json_validation
  COMMAND json_validation_test
  )
