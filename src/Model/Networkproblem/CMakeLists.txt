add_library(networkproblem STATIC Networkproblem.cpp Equationcomponent.cpp)
target_link_libraries(networkproblem PRIVATE problemlayer network)
target_link_libraries(networkproblem PUBLIC exception)
target_include_directories(networkproblem PUBLIC "include")
