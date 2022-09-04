add_library(netfactory STATIC Netfactory.cpp)

target_link_libraries(netfactory PRIVATE aux_json exception componentjsonhelpers)
target_link_libraries(netfactory PUBLIC network componentfactory nlohmann_json)

target_include_directories(netfactory PUBLIC include)
