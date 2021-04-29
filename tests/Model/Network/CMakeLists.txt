add_executable(net_test NetTest.cpp)

target_link_libraries(net_test PUBLIC network)
target_link_libraries(net_test PUBLIC gtest gtest_main gmock)

add_test(
  NAME net_test
  COMMAND net_test
  )
