add_executable(ImplicitOptimizer_test ImplicitOptimizerTest.cpp)
target_include_directories(ImplicitOptimizer_test PUBLIC include)
target_link_libraries(ImplicitOptimizer_test PUBLIC gtest gtest_main gmock interpolatingVector optimizer problemlayer componentclasses)

add_test(
  NAME ImplicitOptimizer_test
  COMMAND ImplicitOptimizer_test
  )



add_executable(constraintJacobian_test ConstraintJacobianTest.cpp)
target_include_directories(constraintJacobian_test PUBLIC include)
target_link_libraries(constraintJacobian_test PUBLIC gtest gtest_main gmock constraintJacobian)

add_test(
  NAME constraintJacobian_test
  COMMAND constraintJacobian_test
  )


# add_executable(UNCONSTRAINED_LAMBDAS unconstrained_lambdas.cpp)
# target_include_directories(UNCONSTRAINED_LAMBDAS PUBLIC include)
# target_link_libraries(UNCONSTRAINED_LAMBDAS PUBLIC
#   gtest_main my_ipopt ipoptwrapper componentclasses)

# add_executable(UNCONSTRAINED_CLASS unconstrained_class.cpp)
# target_include_directories(UNCONSTRAINED_CLASS PUBLIC include)
# target_link_libraries(UNCONSTRAINED_CLASS PUBLIC
#   gtest_main my_ipopt ipoptwrapper componentclasses)


# add_test(
#     NAME UNCONSTRAINED_LAMBDAS
#     COMMAND UNCONSTRAINED_LAMBDAS
# )

# add_test(
#     NAME UNCONSTRAINED_CLASS
#     COMMAND UNCONSTRAINED_CLASS
# )
