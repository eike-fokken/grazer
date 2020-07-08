
add_executable(network_test network_test.cpp)
add_dependencies(network_test network)

target_link_libraries(network_test network)
