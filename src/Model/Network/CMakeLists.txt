add_library(network STATIC Net.cpp Edge.cpp Node.cpp)
target_include_directories(network PUBLIC "${CMAKE_SOURCE_DIR}/include/Model/Network/")
