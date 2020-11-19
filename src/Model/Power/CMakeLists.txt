add_library(power STATIC Powernode.cpp Vphinode.cpp PQnode.cpp PVnode.cpp Transmissionline.cpp)

target_include_directories(power PUBLIC include)
target_link_libraries(power PUBLIC networkproblem)
target_link_libraries(power PRIVATE network matrixhandler exception nlohmann_json)
