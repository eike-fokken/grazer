add_library(matrixhandler STATIC Matrixhandler.cpp Costgradienthandler.cpp)
target_link_libraries(matrixhandler PRIVATE exception)
target_include_directories(matrixhandler PUBLIC "include")
