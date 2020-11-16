add_executable(grazer grazer.cpp)
target_include_directories(grazer PRIVATE "include")
target_link_libraries(grazer PRIVATE nlohmann_json model jsonreader)
