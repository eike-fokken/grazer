add_library(model STATIC Model.cpp Subproblem.cpp)
target_include_directories(model PRIVATE "${CMAKE_SOURCE_DIR}/include/Model")
