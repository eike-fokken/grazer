add_library(network STATIC Net.cpp Edge.cpp Node.cpp)

target_link_libraries(network PRIVATE exception)
target_include_directories(network PUBLIC "include")
