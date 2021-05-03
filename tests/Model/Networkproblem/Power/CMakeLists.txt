add_library(powertest_helpers STATIC powertest_helpers.cpp)
target_link_libraries(powertest_helpers PRIVATE test_helpers)
target_include_directories(powertest_helpers PUBLIC include)


add_executable(power_test PowerTest.cpp)
target_link_libraries(power_test PUBLIC power power_factory netfactory networkproblem matrixhandler test_helpers powertest_helpers)
target_link_libraries(power_test PUBLIC gtest gtest_main gmock)

add_test(
  NAME power_test
  COMMAND power_test
  )


add_executable(stochasticpqnode_test StochasticPQnodeTest.cpp)

target_link_libraries(stochasticpqnode_test PUBLIC power power_factory netfactory networkproblem matrixhandler test_helpers powertest_helpers)
target_link_libraries(stochasticpqnode_test PUBLIC gtest gtest_main gmock)

add_test(
  NAME stochasticpqnode_test
  COMMAND stochasticpqnode_test
  )
