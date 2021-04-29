add_executable(aux_json_test AuxJsonTest.cpp)

target_link_libraries(aux_json_test PUBLIC aux_json test_helpers)
target_link_libraries(aux_json_test PUBLIC gtest gtest_main gmock)

add_test(
  NAME aux_json_test
  COMMAND aux_json_test
  )


