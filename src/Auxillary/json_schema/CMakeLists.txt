
add_library(aux_make_schema STATIC make_schema.cpp)

target_link_libraries(aux_make_schema PRIVATE nlohmann_json aux_unit_conversion exception)

target_include_directories(aux_make_schema PUBLIC include)
