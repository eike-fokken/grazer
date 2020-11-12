add_library(newton STATIC Newtonsolver.cpp)

target_link_libraries(newton PRIVATE matrixhandler)

target_include_directories(newton PUBLIC include)
