add_subdirectory(Power)
add_subdirectory(Gas)

add_library(networkproblem STATIC Networkproblem.cpp Equationcomponent.cpp Netprob_Aux.cpp Choosercontainer.cpp)

# This is needed in order to construct the specific components in Aux.cpp in construct_node
target_link_libraries(networkproblem PRIVATE power gas)

target_link_libraries(networkproblem PRIVATE problemlayer network nlohmann_json mathfunctions aux_json)
target_link_libraries(networkproblem PUBLIC exception)

target_include_directories(networkproblem PUBLIC include)
target_include_directories(networkproblem PRIVATE aux_include)
