add_library(grazer STATIC grazer.cpp)
target_include_directories(grazer PRIVATE "${CMAKE_SOURCE_DIR}/include/Grazer")
target_link_libraries(grazer problem)
