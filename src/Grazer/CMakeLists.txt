add_executable(grazer grazer.cpp)
target_link_libraries(grazer PRIVATE commands aux_schema_generation aux_schema_key_insertion)
target_link_libraries(grazer PUBLIC
  nlohmann_json)

target_compile_definitions(grazer PRIVATE -DGRAZER_VERSION=${GRAZER_VERSION})

add_library(commands STATIC commands.cpp helpers.cpp)
target_link_libraries(commands PRIVATE problemlayer aux_json input_output networkproblem netfactory full_factory interpolatingVector optimization_helpers ipoptwrapper misc)
target_link_libraries(commands PUBLIC
  nlohmann_json)

target_include_directories(commands PUBLIC include)
