add_executable(xml2json EXCLUDE_FROM_ALL xml2json.cpp)

target_link_libraries(xml2json PRIVATE nlohmann_json)
