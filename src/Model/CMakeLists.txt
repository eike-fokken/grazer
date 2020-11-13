
add_subdirectory(Problemlayer)
add_subdirectory(Power)
add_subdirectory(Network)
add_subdirectory(Networkproblem)


add_library(model INTERFACE)
target_sources(model INTERFACE include/model.hpp)
target_link_libraries(model INTERFACE problemlayer network networkproblem)

target_include_directories(model INTERFACE include)
