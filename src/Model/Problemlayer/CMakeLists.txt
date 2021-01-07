add_library(problemlayer STATIC Problem.cpp  Subproblem.cpp Subproblemchooser.cpp Timeevolver.cpp)

target_link_libraries(problemlayer PRIVATE exception matrixhandler newton networkproblem)
target_link_libraries(problemlayer PUBLIC nlohmann_json )
target_include_directories(problemlayer PUBLIC include)
