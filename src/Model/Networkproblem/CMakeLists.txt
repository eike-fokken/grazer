add_subdirectory(Componentclasses)
add_subdirectory(Power)
add_subdirectory(Gas)
add_subdirectory(Gaspowerconnection)

add_library(networkproblem STATIC Networkproblem.cpp)

target_link_libraries(networkproblem PRIVATE network componentclasses)
target_link_libraries(networkproblem PUBLIC exception mathfunctions)

target_include_directories(networkproblem PUBLIC include)
