add_library(power STATIC powernode.cpp Vphinode.cpp PQnode.cpp PVnode.cpp)

target_include_directories(power PRIVATE "include")

target_link_libraries(power PUBLIC model)
