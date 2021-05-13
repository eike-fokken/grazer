add_subdirectory(Full_factory)
add_subdirectory(Gas_factory)
add_subdirectory(Power_factory)

add_library(componentfactory STATIC Componentfactory.cpp)

# This is needed in order to construct the specific components in Aux.cpp in construct_node
target_link_libraries(componentfactory PUBLIC network aux_make_schema aux_schema_validation equationcomponent)
target_include_directories(componentfactory PUBLIC include)
