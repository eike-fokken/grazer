

add_library(controlhelpers STATIC Controlhelpers.cpp)
target_link_libraries(controlhelpers PRIVATE exception)
target_include_directories(controlhelpers PUBLIC include)
