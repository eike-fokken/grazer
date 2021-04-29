add_executable(power_test PowerTest.cpp)

target_link_libraries(power_test PUBLIC power network matrixhandler)
target_link_libraries(power_test PUBLIC gtest gtest_main gmock)

add_test(
  NAME power_test
  COMMAND power_test
  )
