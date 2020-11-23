add_library(newton STATIC Newtonsolver.cpp)

target_link_libraries(newton PRIVATE matrixhandler problemlayer exception testheader)

target_include_directories(newton PUBLIC include)
