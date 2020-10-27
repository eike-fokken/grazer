find_package (Eigen3 3.3 REQUIRED NO_MODULE)






add_subdirectory(Network)

add_library(model STATIC Model.cpp Subproblem.cpp Equationcomponent.cpp)
add_dependencies(model network)

target_include_directories(model PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../include/Model")
target_link_libraries (model PUBLIC Eigen3::Eigen)

