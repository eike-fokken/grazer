add_library(power STATIC Powernode.cpp Vphinode.cpp PQnode.cpp PVnode.cpp Transmissionline.cpp)

target_include_directories(power PRIVATE "include")

target_link_libraries(power PRIVATE network networkproblem matrixhandler)
