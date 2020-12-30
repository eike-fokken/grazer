add_subdirectory(Aux_Executable)

add_executable(grazer grazer.cpp)
target_link_libraries(grazer PRIVATE nlohmann_json problemlayer network jsonreader exception printguard newton aux_executable)
