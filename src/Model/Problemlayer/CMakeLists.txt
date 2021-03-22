add_library(problemlayer STATIC Problem.cpp  Subproblem.cpp Subproblemchooser.cpp Timeevolver.cpp)

target_link_libraries(problemlayer PRIVATE matrixhandler newton networkproblem aux_json netfactory full_factory)
target_link_libraries(problemlayer PUBLIC exception newton)

target_include_directories(problemlayer PUBLIC include)
