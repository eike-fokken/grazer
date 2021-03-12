add_subdirectory(Equationcomponent)
add_subdirectory(Power)
add_subdirectory(Gas)
add_subdirectory(Componentfactory)

add_library(networkproblem STATIC Networkproblem.cpp)

target_link_libraries(networkproblem PRIVATE problemlayer network equationcomponent)
target_link_libraries(networkproblem PUBLIC exception mathfunctions)

target_include_directories(networkproblem PUBLIC include)
