find_package (Eigen3 3.3 REQUIRED NO_MODULE)



add_subdirectory(Network)
add_subdirectory(Power)



add_library(model STATIC Model.cpp Subproblem.cpp Equationcomponent.cpp)
add_dependencies(model network)

target_include_directories(model PUBLIC "include")
target_link_libraries (model PUBLIC Eigen3::Eigen)
target_link_libraries (model INTERFACE network)






