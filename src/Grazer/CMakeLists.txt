add_library(grazer STATIC grazer.cpp)
target_include_directories(grazer PRIVATE "include")
target_link_libraries(grazer problem)
