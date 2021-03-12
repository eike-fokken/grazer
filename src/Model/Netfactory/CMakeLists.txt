add_library(netfactory STATIC Networkproblem_helpers.cpp)

target_link_libraries(netfactory PUBLIC network aux_json componentfactory exception)

target_include_directories(netfactory PUBLIC include)
