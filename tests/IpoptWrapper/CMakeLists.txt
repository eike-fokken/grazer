add_executable(Wrapper_test WrapperTest.cpp)
target_include_directories(Wrapper_test PUBLIC include)
target_link_libraries(Wrapper_test PUBLIC gtest gtest_main gmock interpolatingVector ipoptwrapper problemlayer componentclasses)

add_test(
  NAME Wrapper_test
  COMMAND Wrapper_test
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
