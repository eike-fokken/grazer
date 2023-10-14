add_library(misc STATIC Misc.cpp)

target_link_libraries(misc PUBLIC exception)

target_include_directories(misc PUBLIC include)
