
add_subdirectory(Problemlayer)
add_subdirectory(Network)
add_subdirectory(Networkproblem)


add_library(model INTERFACE)
target_link_libraries(model INTERFACE power problemlayer network networkproblem )

target_include_directories(model INTERFACE include)
