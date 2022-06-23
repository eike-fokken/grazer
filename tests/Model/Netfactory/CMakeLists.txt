add_executable(netfactory_test NetfactoryTest.cpp)

target_link_libraries(netfactory_test PUBLIC netfactory power_factory power componentjsonhelpers)
target_link_libraries(netfactory_test PUBLIC gtest gtest_main gmock)

add_test(
  NAME netfactory_test
  COMMAND netfactory_test
  )
