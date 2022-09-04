add_library(aux_json STATIC Aux_json.cpp)
target_link_libraries(aux_json PRIVATE  exception)
target_link_libraries(aux_json PUBLIC
  nlohmann_json)

target_include_directories(aux_json PUBLIC "include")
