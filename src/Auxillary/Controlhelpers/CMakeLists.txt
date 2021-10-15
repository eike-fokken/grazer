

add_library(controlhelpers STATIC Controlhelpers.cpp)
target_link_libraries(controlhelpers PRIVATE exception mathfunctions)
target_link_libraries(controlhelpers PUBLIC timedata)
target_include_directories(controlhelpers PUBLIC include)
