add_executable(problem_test ProblemTest.cpp)

target_link_libraries(problem_test PUBLIC problemlayer subproblem)
target_link_libraries(problem_test PUBLIC gtest gtest_main gmock)
target_link_libraries(problem_test PUBLIC test_helpers)
target_include_directories(problem_test PUBLIC include)

add_test(
  NAME problem_test
  COMMAND problem_test
  )

