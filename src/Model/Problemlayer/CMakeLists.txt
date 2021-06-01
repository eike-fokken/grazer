add_library(problemlayer STATIC Problem.cpp Subproblemchooser.cpp Timeevolver.cpp)

target_link_libraries(problemlayer PRIVATE matrixhandler newton networkproblem aux_json netfactory full_factory aux_make_schema)
target_link_libraries(problemlayer PUBLIC exception newton subproblem)

target_include_directories(problemlayer PUBLIC include)
