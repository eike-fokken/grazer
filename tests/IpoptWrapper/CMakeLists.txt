


add_executable(constrainedJacobian_test ConstrainedJacobianTest.cpp)
target_include_directories(constrainedJacobian_test PUBLIC include)
target_link_libraries(constrainedJacobian_test PUBLIC gtest gtest_main interpolatingVector ipoptwrapper)

add_test(
  NAME constrainedJacobian_test
  COMMAND constrainedJacobian_test
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
