add_executable(net_test NetTest.cpp)

target_link_libraries(net_test PUBLIC network networkproblem )
target_link_libraries(net_test PUBLIC gtest gtest_main gmock)

add_executable(model_test ModelTest.cpp)

target_link_libraries(model_test PUBLIC problemlayer )
target_link_libraries(model_test PUBLIC gtest gtest_main gmock)



add_executable(power_test PowerTest.cpp)

target_link_libraries(power_test PUBLIC problemlayer power networkproblem network exception newton matrixhandler  power)
target_link_libraries(power_test PUBLIC gtest gtest_main gmock)



add_executable(newton_test NewtonsolverTest.cpp)

target_link_libraries(newton_test PUBLIC network exception newton matrixhandler problemlayer )
target_link_libraries(newton_test PUBLIC gtest gtest_main gmock)
target_include_directories(newton_test PUBLIC include)


add_executable(isothermaleulerequation_test IsothermaleulerequationTest.cpp)

target_link_libraries(isothermaleulerequation_test PUBLIC balancelaw)
target_link_libraries(isothermaleulerequation_test PUBLIC gtest gtest_main gmock)


add_executable(implicitboxscheme_test ImplicitboxschemeTest.cpp)

target_link_libraries(implicitboxscheme_test PUBLIC balancelaw scheme)
target_link_libraries(implicitboxscheme_test PUBLIC gtest gtest_main gmock)


add_executable(gas_test GasTest.cpp)

target_link_libraries(gas_test PUBLIC problemlayer power networkproblem network exception newton matrixhandler  power gas coloroutput misc)
target_link_libraries(gas_test PUBLIC gtest gtest_main gmock)


add_library(testheader INTERFACE)
target_link_libraries(testheader INTERFACE  matrixhandler problemlayer gtest gtest_main gmock)
target_include_directories(testheader INTERFACE include)

add_test(
    NAME net_test
    COMMAND net_test
    )
  add_test(
    NAME model_test
    COMMAND model_test
    )
add_test(
  NAME power_test
  COMMAND power_test
  )
add_test(
  NAME newton_test
  COMMAND newton_test
  )

add_test(
  NAME isothermaleulerequation_test
  COMMAND isothermaleulerequation_test
  )
add_test(
  NAME implicitboxscheme_test
  COMMAND implicitboxscheme_test
  )

add_test(
  NAME gas_test
  COMMAND gas_test
  )
