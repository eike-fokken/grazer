add_executable(boundary_test BoundaryTest.cpp)
target_link_libraries(boundary_test PUBLIC componentclasses )
target_link_libraries(boundary_test PUBLIC gtest gtest_main gmock)

add_executable(initial_test InitialTest.cpp )
target_link_libraries(initial_test PUBLIC componentclasses )
target_link_libraries(initial_test PUBLIC gtest gtest_main gmock)

add_executable(control_test ControlTest.cpp )
target_link_libraries(control_test PUBLIC componentclasses )
target_link_libraries(control_test PUBLIC gtest gtest_main gmock)

add_executable(valuemap_test ValuemapTest.cpp )
target_link_libraries(valuemap_test PUBLIC componentclasses )
target_link_libraries(valuemap_test PUBLIC gtest gtest_main gmock)

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
  NAME valuemap_test
  COMMAND valuemap_test
  )
