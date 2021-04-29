add_subdirectory(Testproblem)

add_executable(newton_test NewtonsolverTest.cpp)

target_link_libraries(newton_test PUBLIC newton testproblem)
target_link_libraries(newton_test PUBLIC gtest gtest_main gmock)


add_test(
  NAME newton_test
  COMMAND newton_test
  )
