add_executable(isothermaleulerequation_test IsothermaleulerequationTest.cpp)

target_link_libraries(isothermaleulerequation_test PUBLIC balancelaw mathfunctions)
target_link_libraries(isothermaleulerequation_test PUBLIC gtest gtest_main gmock)


add_test(
  NAME isothermaleulerequation_test
  COMMAND isothermaleulerequation_test
  )
