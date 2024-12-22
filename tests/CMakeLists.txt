add_compile_options("-Wno-switch-default")

add_subdirectory(Model)
add_subdirectory(Newtonsolver)
add_subdirectory(Auxillary)
add_subdirectory(IpoptWrapper)
add_subdirectory(Optimization_helpers)


add_library(test_helpers STATIC Equationcomponent_test_helpers.cpp test_io_helper.cpp)

target_link_libraries(test_helpers PUBLIC netfactory networkproblem randutils)
target_link_libraries(test_helpers PUBLIC gtest gtest_main gmock)

target_include_directories(test_helpers PUBLIC include)





add_executable(playground_test PlaygroundTest.cpp)
target_link_libraries(playground_test PUBLIC balancelaw scheme)
target_link_libraries(playground_test PUBLIC gtest gtest_main gmock)

add_test(
  NAME playground_test
  COMMAND playground_test
  )
