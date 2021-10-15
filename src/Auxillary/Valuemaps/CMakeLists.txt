add_library(valuemap STATIC Valuemap.cpp)

target_link_libraries(valuemap PUBLIC exception mathfunctions)
target_include_directories(valuemap PUBLIC include)
