add_executable(implicitboxscheme_test ImplicitboxschemeTest.cpp)

target_link_libraries(implicitboxscheme_test PUBLIC balancelaw scheme)
target_link_libraries(implicitboxscheme_test PUBLIC gtest gtest_main gmock)


add_test(
  NAME implicitboxscheme_test
  COMMAND implicitboxscheme_test
  )
