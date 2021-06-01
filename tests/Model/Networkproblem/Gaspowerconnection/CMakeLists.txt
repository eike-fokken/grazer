add_executable(gaspowerconnection_test GaspowerconnectionTest.cpp)
target_link_libraries(gaspowerconnection_test PUBLIC full_factory gaspowerconnection power netfactory networkproblem matrixhandler test_helpers)
target_link_libraries(gaspowerconnection_test PUBLIC gtest gtest_main gmock)


add_test(
  NAME gaspowerconnection
  COMMAND gaspowerconnection_test
  )
