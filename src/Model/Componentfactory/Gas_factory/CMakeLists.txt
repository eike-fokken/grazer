add_library(gas_factory STATIC Gas_factory.cpp)

# This is needed in order to construct the specific components in Aux.cpp in construct_node
target_link_libraries(gas_factory PRIVATE componentfactory gas network)

target_include_directories(gas_factory PUBLIC include)
