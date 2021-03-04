add_executable(grazer grazer.cpp)
target_link_libraries(grazer PRIVATE  problemlayer network aux_json exception newton input_output)
