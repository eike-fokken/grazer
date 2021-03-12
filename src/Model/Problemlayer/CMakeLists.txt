add_library(problemlayer STATIC Problem.cpp  Subproblem.cpp Subproblemchooser.cpp Timeevolver.cpp)

target_link_libraries(problemlayer PRIVATE exception matrixhandler newton networkproblem aux_json netfactory)
target_link_libraries(problemlayer PUBLIC  newton)

target_include_directories(problemlayer PUBLIC include)
