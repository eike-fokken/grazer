add_executable(aux_unit_conversion_test AuxUnitConversionTest.cpp)
target_link_libraries(aux_unit_conversion_test PUBLIC aux_unit_conversion nlohmann_json)
target_link_libraries(aux_unit_conversion_test PUBLIC gtest gtest_main gmock)


add_test(
  NAME aux_unit_conversion
  COMMAND aux_unit_conversion_test
  )
