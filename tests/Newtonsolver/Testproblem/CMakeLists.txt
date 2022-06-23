add_library(testproblem STATIC Testproblem.cpp)

target_link_libraries(testproblem PUBLIC matrixhandler componentclasses gtest gtest_main gmock)
target_include_directories(testproblem PUBLIC include)
