
add_executable(adjoint_equations_solver_test adjoint_equations_solver.cpp)
target_link_libraries(adjoint_equations_solver_test PUBLIC gtest gtest_main gmock optimization_helpers)

add_test(
  NAME adjoint_equations_solver_test 
  COMMAND adjoint_equations_solver_test 
  )




add_executable(controlStateMap_test ControlStateMapTest.cpp)
target_link_libraries(controlStateMap_test PUBLIC gtest gtest_main gmock optimization_helpers)

add_test(
  NAME controlStateMap_test 
  COMMAND controlStateMap_test 
  )
