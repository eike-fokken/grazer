add_executable(grazer grazer.cpp)
target_link_libraries(grazer PRIVATE CLI11::CLI11 commands aux_schema_generation aux_schema_key_insertion)

add_library(commands STATIC commands.cpp)
target_link_libraries(commands PRIVATE problemlayer aux_json input_output)
target_include_directories(commands PUBLIC include)
