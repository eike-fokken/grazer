add_subdirectory(Full_factory)

add_library(componentfactory INTERFACE)

# This is needed in order to construct the specific components in Aux.cpp in construct_node
target_link_libraries(componentfactory INTERFACE network)
target_include_directories(componentfactory INTERFACE include)
