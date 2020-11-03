add_executable(net_test NetTest.cpp)
add_dependencies(net_test model)
target_link_libraries(net_test PRIVATE model)

target_link_libraries(net_test PUBLIC network gtest gtest_main gmock)




add_test(
    NAME net_test
    COMMAND net_test
)
