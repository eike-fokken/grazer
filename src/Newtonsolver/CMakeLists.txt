add_library(newton STATIC Newtonsolver.cpp)

target_link_libraries(newton PUBLIC matrixhandler problemlayer Eigen3::Eigen)

target_include_directories(newton PUBLIC include)
