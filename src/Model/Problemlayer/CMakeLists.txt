add_library(problemlayer STATIC Problem.cpp  Subproblem.cpp)

target_link_libraries(problemlayer PRIVATE exception matrixhandler nlohmann_json)
target_include_directories(problemlayer PUBLIC "include")
