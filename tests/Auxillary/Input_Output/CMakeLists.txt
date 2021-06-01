add_executable(input_output_test InputOutputTest.cpp)

target_link_libraries(input_output_test PUBLIC input_output)
target_link_libraries(input_output_test PUBLIC gtest gtest_main gmock test_helpers)


add_test(
  NAME input_output_test
  COMMAND input_output_test
  )
