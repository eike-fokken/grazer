add_library(testproblem STATIC Testproblem.cpp)


target_link_libraries(testproblem PRIVATE matrixhandler)
target_include_directories(testproblem PUBLIC include)
