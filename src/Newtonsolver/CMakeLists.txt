add_library(newtonsolver STATIC Newtonsolver.cpp)
target_link_libraries(newtonsolver PRIVATE problemlayer matrixhandler)
target_include_directories(newtonsolver PUBLIC "include")
