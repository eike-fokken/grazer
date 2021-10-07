add_executable(grazer grazer.cpp)
target_link_libraries(grazer PRIVATE commands aux_schema_generation aux_schema_key_insertion)
target_compile_definitions(grazer PRIVATE -DGRAZER_VERSION=${GRAZER_VERSION})

add_library(commands STATIC commands.cpp)
target_link_libraries(commands PRIVATE problemlayer aux_json input_output networkproblem netfactory full_factory)
target_include_directories(commands PUBLIC include)
