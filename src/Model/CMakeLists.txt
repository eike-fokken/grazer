
add_subdirectory(Problemlayer)
add_subdirectory(Power)
add_subdirectory(Network)
add_subdirectory(Generalnetworkproblem)


add_library(model INTERFACE)
target_link_libraries(model INTERFACE problemlayer network generalnetworkproblem)

