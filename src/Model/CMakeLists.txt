add_subdirectory(Network)

add_library(model STATIC Model.cpp Subproblem.cpp)
add_dependencies(model network)

target_include_directories(model PUBLIC "${CMAKE_SOURCE_DIR}/include/Model")
