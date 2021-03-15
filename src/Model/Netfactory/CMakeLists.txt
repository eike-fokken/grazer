add_library(netfactory STATIC Netfactory.cpp)

target_link_libraries(netfactory PRIVATE aux_json exception)
target_link_libraries(netfactory PUBLIC network componentfactory)

target_include_directories(netfactory PUBLIC include)
