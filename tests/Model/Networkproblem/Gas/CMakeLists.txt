add_executable(gas_test GasTest.cpp)
target_link_libraries(gas_test PUBLIC gas_factory gas netfactory networkproblem matrixhandler test_helpers interpolatingVector)
target_link_libraries(gas_test PUBLIC gtest gtest_main gmock)


add_test(
  NAME gas_test
  COMMAND gas_test
  )
