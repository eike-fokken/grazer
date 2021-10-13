add_library(problemlayer STATIC Timeevolver.cpp Optimizer.cpp)

target_link_libraries(problemlayer PRIVATE matrixhandler newton networkproblem aux_json netfactory full_factory aux_make_schema ipoptwrapper controlhelpers)
target_link_libraries(problemlayer PUBLIC exception newton)

target_include_directories(problemlayer PUBLIC include)
