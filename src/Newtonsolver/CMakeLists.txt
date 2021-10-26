add_library(newton STATIC Newtonsolver.cpp)

target_link_libraries(newton PRIVATE matrixhandler exception)
target_link_libraries(newton PUBLIC componentclasses)

target_include_directories(newton PUBLIC include)
