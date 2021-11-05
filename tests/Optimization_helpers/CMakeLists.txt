
add_executable(adjoint_equations_solver_test adjoint_equations_solver.cpp)
target_link_libraries(adjoint_equations_solver_test PUBLIC gtest gtest_main gmock optimization_helpers)

add_test(
  NAME adjoint_equations_solver_test 
  COMMAND adjoint_equations_solver_test 
  )




add_executable(controlStateCache_test ControlStateCacheTest.cpp)
target_link_libraries(controlStateCache_test PUBLIC gtest gtest_main gmock optimization_helpers)

add_test(
  NAME controlStateCache_test 
  COMMAND controlStateCache_test 
  )


add_executable(optimization_helpers_test Optimization_helpersTest.cpp)
target_link_libraries(optimization_helpers_test PUBLIC gtest gtest_main gmock optimization_helpers)

add_test(
  NAME optimization_helpers_test 
  COMMAND optimization_helpers_test 
  )
