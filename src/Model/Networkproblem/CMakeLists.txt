add_library(networkproblem STATIC Networkproblem.cpp Equationcomponent.cpp Boundelement.cpp)
target_link_libraries(networkproblem PRIVATE problemlayer network exception)
target_include_directories(networkproblem PUBLIC "include")
