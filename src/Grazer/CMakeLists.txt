add_executable(grazer grazer.cpp)
target_link_libraries(grazer PRIVATE nlohmann_json problemlayer network aux_json exception newton input_output)
