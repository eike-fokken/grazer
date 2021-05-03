add_library(power_factory STATIC Power_factory.cpp)

# This is needed in order to construct the specific components in Aux.cpp in construct_node
target_link_libraries(power_factory PRIVATE power network)
target_link_libraries(power_factory PUBLIC componentfactory)

target_include_directories(power_factory PUBLIC include)
