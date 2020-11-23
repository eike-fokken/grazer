add_executable(net_test NetTest.cpp)

target_link_libraries(net_test PRIVATE model network exception newton matrixhandler nlohmann_json)

target_link_libraries(net_test PUBLIC gtest gtest_main gmock)
target_include_directories(net_test PRIVATE include)


add_executable(power_test PowerTest.cpp)

target_link_libraries(power_test PRIVATE model network exception newton matrixhandler nlohmann_json power)
target_link_libraries(power_test PUBLIC gtest gtest_main gmock)
target_include_directories(power_test PRIVATE include)



add_executable(newton_test NewtonsolverTest.cpp)

target_link_libraries(newton_test PUBLIC network exception newton matrixhandler problemlayer nlohmann_json)
target_link_libraries(newton_test PUBLIC gtest gtest_main gmock)
target_include_directories(newton_test PRIVATE include)


add_library(testheader INTERFACE)
target_include_directories(testheader INTERFACE include)

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
