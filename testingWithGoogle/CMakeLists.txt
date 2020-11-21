add_executable(net_test NetTest.cpp)

target_link_libraries(net_test PRIVATE model network exception newton matrixhandler nlohmann_json)
target_link_libraries(net_test PUBLIC gtest gtest_main gmock)



add_executable(power_test PowerTest.cpp)

target_link_libraries(power_test PRIVATE model network exception newton matrixhandler nlohmann_json power)
target_link_libraries(power_test PUBLIC gtest gtest_main gmock)




add_executable(newton_test NewtonsolverTest.cpp)

target_link_libraries(newton_test PUBLIC network exception newton matrixhandler problemlayer nlohmann_json)
target_link_libraries(newton_test PUBLIC gtest gtest_main gmock)


add_test(
    NAME net_test
    COMMAND net_test
)
add_test(
  NAME power_test
  COMMAND power_test
  )
add_test(
  NAME newton_test
  COMMAND newton_test
  )
