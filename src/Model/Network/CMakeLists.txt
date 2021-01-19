add_library(network STATIC Net.cpp Edge.cpp Node.cpp Idobject.cpp)

target_link_libraries(network PRIVATE exception nlohmann_json)
target_include_directories(network PUBLIC "include")
