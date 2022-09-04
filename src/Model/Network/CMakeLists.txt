add_library(network STATIC Net.cpp Edge.cpp Node.cpp Idobject.cpp)

target_link_libraries(network PRIVATE exception aux_make_schema)
target_link_libraries(network PUBLIC
  nlohmann_json)

target_include_directories(network PUBLIC "include")
