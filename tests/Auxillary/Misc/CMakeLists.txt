add_executable(aux_misc_test AuxMiscTest.cpp)

target_link_libraries(aux_misc_test PUBLIC misc test_helpers)
target_link_libraries(aux_misc_test PUBLIC gtest gtest_main gmock)

add_test(
  NAME aux_misc_test
  COMMAND aux_misc_test
)