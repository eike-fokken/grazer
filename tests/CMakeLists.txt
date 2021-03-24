add_library(eqcomp_helpers STATIC Equationcomponent_test_helpers.cpp)
target_include_directories(eqcomp_helpers PUBLIC include)



add_executable(net_test NetTest.cpp)

target_link_libraries(net_test PUBLIC network)
target_link_libraries(net_test PUBLIC gtest gtest_main gmock)

add_executable(model_test ModelTest.cpp)

target_link_libraries(model_test PUBLIC problemlayer subproblem)
target_link_libraries(model_test PUBLIC gtest gtest_main gmock)

add_executable(boundary_test BoundaryTest.cpp)

target_link_libraries(boundary_test PUBLIC equationcomponent )
target_link_libraries(boundary_test PUBLIC gtest gtest_main gmock)

add_executable(initial_test InitialTest.cpp )

target_link_libraries(initial_test PUBLIC equationcomponent )
target_link_libraries(initial_test PUBLIC gtest gtest_main gmock)

add_executable(control_test ControlTest.cpp )

target_link_libraries(control_test PUBLIC equationcomponent )
target_link_libraries(control_test PUBLIC gtest gtest_main gmock)

add_executable(power_test PowerTest.cpp)

target_link_libraries(power_test PUBLIC power network matrixhandler)
target_link_libraries(power_test PUBLIC gtest gtest_main gmock)

add_executable(newton_test NewtonsolverTest.cpp)

target_link_libraries(newton_test PUBLIC network exception newton matrixhandler problemlayer )
target_link_libraries(newton_test PUBLIC gtest gtest_main gmock)
target_include_directories(newton_test PUBLIC include)

add_executable(isothermaleulerequation_test IsothermaleulerequationTest.cpp)

target_link_libraries(isothermaleulerequation_test PUBLIC balancelaw mathfunctions)
target_link_libraries(isothermaleulerequation_test PUBLIC gtest gtest_main gmock)


add_executable(implicitboxscheme_test ImplicitboxschemeTest.cpp)

target_link_libraries(implicitboxscheme_test PUBLIC balancelaw scheme)
target_link_libraries(implicitboxscheme_test PUBLIC gtest gtest_main gmock)


add_executable(gas_test GasTest.cpp)
target_link_libraries(gas_test PUBLIC gas_factory gas netfactory networkproblem matrixhandler eqcomp_helpers)
target_link_libraries(gas_test PUBLIC gtest gtest_main gmock)
target_include_directories(gas_test PRIVATE include)


add_executable(gaspowerconnection_test GaspowerconnectionTest.cpp)
target_link_libraries(gaspowerconnection_test PUBLIC full_factory gaspowerconnection power netfactory networkproblem matrixhandler eqcomp_helpers)
target_link_libraries(gaspowerconnection_test PUBLIC gtest gtest_main gmock)
target_include_directories(gaspowerconnection_test PRIVATE include)



add_executable(json_validation_test validationTest.cpp)

target_link_libraries(json_validation_test PUBLIC json_validation nlohmann_json)
target_link_libraries(json_validation_test PUBLIC gtest gtest_main gmock)

add_executable(aux_unit_conversion_test AuxUnitConversionTest.cpp)

target_link_libraries(aux_unit_conversion_test PUBLIC aux_unit_conversion nlohmann_json)
target_link_libraries(aux_unit_conversion_test PUBLIC gtest gtest_main gmock)

add_executable(aux_json_test AuxJsonTest.cpp)

target_link_libraries(aux_json_test PUBLIC aux_json)
target_link_libraries(aux_json_test PUBLIC gtest gtest_main gmock)
target_include_directories(aux_json_test PUBLIC include)

add_executable(input_output_test InputOutputTest.cpp)

target_link_libraries(input_output_test PUBLIC input_output)
target_link_libraries(input_output_test PUBLIC gtest gtest_main gmock)
target_include_directories(input_output_test PUBLIC include)

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
    NAME boundary_test
    COMMAND boundary_test
    )
  add_test(
    NAME initial_test
    COMMAND initial_test
    )
  add_test(
    NAME control_test
    COMMAND control_test
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

add_test(
  NAME gaspowerconnection
  COMMAND gaspowerconnection_test
  )


add_test(
  NAME aux_unit_conversion
  COMMAND aux_unit_conversion_test
  )

add_test(
  NAME json_validation
  COMMAND json_validation_test
)
add_test(
  NAME aux_json_test
  COMMAND aux_json_test
)
add_test(
  NAME input_output_test
  COMMAND input_output_test
)
