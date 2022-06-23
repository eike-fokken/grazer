add_executable(interpolatingVector_test InterpolatingVectorTest.cpp)

target_link_libraries(interpolatingVector_test PUBLIC interpolatingVector)
target_link_libraries(interpolatingVector_test PUBLIC gtest gtest_main gmock test_helpers)


add_test(
  NAME interpolatingVector_test
  COMMAND interpolatingVector_test
  )
