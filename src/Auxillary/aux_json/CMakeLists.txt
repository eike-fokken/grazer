add_library(aux_json STATIC Aux_json.cpp)
target_link_libraries(aux_json PRIVATE nlohmann_json exception)
target_include_directories(aux_json PUBLIC "include")
