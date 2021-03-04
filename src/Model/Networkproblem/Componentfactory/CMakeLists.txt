add_library(componentfactory STATIC Componentfactory.cpp)

# This is needed in order to construct the specific components in Aux.cpp in construct_node
target_link_libraries(componentfactory PRIVATE power gas  network)

target_include_directories(componentfactory PUBLIC include)
