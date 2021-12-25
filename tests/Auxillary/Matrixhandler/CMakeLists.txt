add_executable(matrixhandler_test MatrixhandlerTest.cpp)

target_link_libraries(matrixhandler_test PUBLIC matrixhandler)
target_link_libraries(matrixhandler_test PUBLIC gtest gtest_main gmock )


add_test(
  NAME matrixhandler_test
  COMMAND matrixhandler_test
  )
