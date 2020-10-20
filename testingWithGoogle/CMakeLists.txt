add_executable(net_test NetTest.cpp)
add_dependencies(net_test model)

target_link_libraries(net_test PUBLIC network gtest gtest_main)

target_include_directories(net_test PUBLIC "${PROJECT_SOURCE_DIR}/include/Model/Network")


add_test(
    NAME net_test
    COMMAND net_test
)
