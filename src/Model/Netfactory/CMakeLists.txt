add_library(netfactory STATIC Netfactory.cpp)

target_link_libraries(netfactory PRIVATE aux_json componentfactory exception)
target_link_libraries(netfactory PUBLIC network)

target_include_directories(netfactory PUBLIC include)
