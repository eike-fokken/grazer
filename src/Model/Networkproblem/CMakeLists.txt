add_subdirectory(Equationcomponent)
add_subdirectory(Power)
add_subdirectory(Gas)

add_library(networkproblem STATIC Networkproblem.cpp)

target_link_libraries(networkproblem PRIVATE network equationcomponent)
target_link_libraries(networkproblem PUBLIC exception mathfunctions subproblem)

target_include_directories(networkproblem PUBLIC include)
