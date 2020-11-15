add_library(jsonreader STATIC Jsonreader.cpp)

target_link_libraries(jsonreader PRIVATE problemlayer nlohmann_json network exception)

target_include_directories(jsonreader PUBLIC "include")
