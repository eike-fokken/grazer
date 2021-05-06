add_subdirectory(Model)
add_subdirectory(Newtonsolver)
add_subdirectory(Auxillary)
add_subdirectory(validation)


add_library(test_helpers STATIC Equationcomponent_test_helpers.cpp test_io_helper.cpp)

target_link_libraries(test_helpers PUBLIC netfactory networkproblem randutils)
target_link_libraries(test_helpers PUBLIC gtest gtest_main gmock)

target_include_directories(test_helpers PUBLIC include)

add_executable(netfactory_test NetfactoryTest.cpp)

target_link_libraries(netfactory_test PUBLIC netfactory)
target_link_libraries(netfactory_test PUBLIC gtest gtest_main gmock)
target_include_directories(netfactory_test PUBLIC include)

add_test(
  NAME netfactory_test
  COMMAND netfactory_test
  )
