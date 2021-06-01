add_executable(normaldistribution_test NormaldistributionTest.cpp)

target_link_libraries(normaldistribution_test PUBLIC stochastics)
target_link_libraries(normaldistribution_test PUBLIC gtest gtest_main gmock)

add_test(
  NAME normaldistribution_test
  COMMAND normaldistribution_test
  )
