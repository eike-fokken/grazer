add_executable(net_test NetTest.cpp)

target_link_libraries(net_test PRIVATE model exception)
target_link_libraries(net_test PRIVATE network gtest gtest_main)




add_test(
    NAME net_test
    COMMAND net_test
)
