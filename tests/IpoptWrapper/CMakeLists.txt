add_executable(HS071 hs071.cpp)
target_link_libraries(HS071 PUBLIC gtest_main my_ipopt ipoptwrapper)

add_executable(UNCONSTRAINED_LAMBDAS unconstrained_lambdas.cpp)
target_link_libraries(UNCONSTRAINED_LAMBDAS PUBLIC
  gtest_main my_ipopt ipoptwrapper)

add_executable(UNCONSTRAINED_CLASS unconstrained_class.cpp)
target_link_libraries(UNCONSTRAINED_CLASS PUBLIC
  gtest_main my_ipopt ipoptwrapper)

add_test(
    NAME HS071
    COMMAND HS071
)

add_test(
    NAME UNCONSTRAINED_LAMBDAS
    COMMAND UNCONSTRAINED_LAMBDAS
)

add_test(
    NAME UNCONSTRAINED_CLASS
    COMMAND UNCONSTRAINED_CLASS
)
