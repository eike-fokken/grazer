add_executable(controlhelpers_test ControlhelpersTest.cpp)

target_link_libraries(controlhelpers_test PUBLIC controlhelpers)
target_link_libraries(controlhelpers_test PUBLIC gtest gtest_main gmock test_helpers)


add_test(
  NAME controlhelpers_test
  COMMAND controlhelpers_test
  )
