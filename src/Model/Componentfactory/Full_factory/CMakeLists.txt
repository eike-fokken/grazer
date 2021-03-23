add_library(full_factory STATIC Full_factory.cpp)

# This is needed in order to construct the specific components in Aux.cpp in construct_node
target_link_libraries(full_factory PRIVATE componentfactory power gas gaspowerconnection network)

target_include_directories(full_factory PUBLIC include)
